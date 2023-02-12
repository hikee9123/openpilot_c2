from pickle import TRUE
from cereal import car, log
from common.realtime import DT_CTRL
from common.numpy_fast import clip, interp
from common.conversions import Conversions as CV
from selfdrive.car import apply_std_steer_torque_limits
from selfdrive.car.hyundai.hyundaican import create_lkas11, create_clu11, create_lfahda_mfc, create_mdps12, create_hda_mfc, create_scc12
from selfdrive.car.hyundai import hyundaican
from selfdrive.car.hyundai.values import Buttons, CarControllerParams, CAR, FEATURES
from opendbc.can.packer import CANPacker

from selfdrive.car.hyundai.navicontrol  import NaviControl

VisualAlert = car.CarControl.HUDControl.VisualAlert
LongCtrlState = car.CarControl.Actuators.LongControlState
LaneChangeState = log.LateralPlan.LaneChangeState


import common.loger as trace1

class CarController():
  def __init__(self, dbc_name, CP, VM):
    self.CP = CP
    self.params = CarControllerParams(CP)
    self.packer = CANPacker(dbc_name)
    self.frame = 0

    self.apply_steer_last = 0
    self.car_fingerprint = CP.carFingerprint
    self.last_resume_frame = 0
    self.accel = 0

    self.resume_cnt = 0



    self.lkas11_cnt = 0
    self.scc12_cnt = 0
    
    self.NC = NaviControl(self.params, CP)
    self.gas = 0
    self.stop_cnt = 0


    # hud
    self.hud_timer_alert = 0
    self.hud_timer_left = 0
    self.hud_timer_right = 0
    self.steer_timer_apply_torque = 1.0
    self.DT_STEER = 0.005             # 0.01 1sec, 0.005  2sec
    self.scc_live = not CP.radarOffCan




  def process_hud_alert(self, enabled, c, CS ):
    visual_alert = c.hudControl.visualAlert
    left_lane = c.hudControl.leftLaneVisible
    right_lane = c.hudControl.rightLaneVisible


    if CS.out.steeringPressed:
      self.hud_timer_alert = 0
    elif CS.clu_Vanz > 30:
      sys_warning = (visual_alert in (VisualAlert.steerRequired, VisualAlert.ldw))

      if sys_warning:
        self.hud_timer_alert = 500

    if left_lane:
      self.hud_timer_left = 100

    if right_lane:
      self.hud_timer_right = 100

    if self.hud_timer_alert:
      sys_warning = True
      self.hud_timer_alert -= 1
    else:
      sys_warning = False


    if self.hud_timer_left:
      self.hud_timer_left -= 1
 
    if self.hud_timer_right:
      self.hud_timer_right -= 1


    # initialize to no line visible
    sys_state = 1
    if self.hud_timer_left and self.hud_timer_right or sys_warning:  # HUD alert only display when LKAS status is active
      if enabled: # or sys_warning:
        sys_state = 3
      else:
        sys_state = 4
    elif self.hud_timer_left:
      sys_state = 5
    elif self.hud_timer_right:
      sys_state = 6

    return sys_warning, sys_state


  
  def smooth_steer( self, apply_torque, CS ):
    if self.CP.smoothSteer.maxSteeringAngle and abs(CS.out.steeringAngleDeg) > self.CP.smoothSteer.maxSteeringAngle:
      if self.CP.smoothSteer.maxDriverAngleWait and CS.out.steeringPressed:
        self.steer_timer_apply_torque -= self.CP.smoothSteer.maxDriverAngleWait # 0.002 #self.DT_STEER   # 0.01 1sec, 0.005  2sec   0.002  5sec
      elif self.CP.smoothSteer.maxSteerAngleWait:
        self.steer_timer_apply_torque -= self.CP.smoothSteer.maxSteerAngleWait # 0.001  # 10 sec
    elif self.CP.smoothSteer.driverAngleWait and CS.out.steeringPressed:
      self.steer_timer_apply_torque -= self.CP.smoothSteer.driverAngleWait #0.001
    else:
      if self.steer_timer_apply_torque >= 1:
          return int(round(float(apply_torque)))
      self.steer_timer_apply_torque += self.DT_STEER

    if self.steer_timer_apply_torque < 0:
      self.steer_timer_apply_torque = 0
    elif self.steer_timer_apply_torque > 1:
      self.steer_timer_apply_torque = 1

    apply_torque *= self.steer_timer_apply_torque

    return  int(round(float(apply_torque)))



  def update_debug(self, CS, c, apply_steer ):
    actuators = c.actuators
    vFuture = c.hudControl.vFuture * 3.6

    
    str_log1 = 'TG={:.1f}  DIST={:.2f} {:.2f} NC={}'.format(  apply_steer, CS.lead_distance, CS.out.vEgoCluster, self.NC.log_msg )
    trace1.printf2( '{}'.format( str_log1 ) )

    if actuators.longControlState == LongCtrlState.off:
      scc_log2 = 'off'
    elif actuators.longControlState == LongCtrlState.pid:
      scc_log2 = 'pid'
    elif actuators.longControlState == LongCtrlState.stopping:
      scc_log2 = 'stop'
    elif actuators.longControlState == LongCtrlState.starting:
      scc_log2 = 'start'


    str_log1 = 'MODE={:.0f} vF={:.1f}  aRV={:.2f} , {:.2f}, {}'.format( CS.cruise_set_mode, vFuture, CS.aReqValue, self.accel , scc_log2 )
    trace1.printf3( '{}'.format( str_log1 ) )
  

  
  def update_scc12(self, can_sends,  c, CS ):
    actuators = c.actuators
    enabled = c.enabled and CS.out.cruiseState.accActive
    accel = CS.aReqValue
    stopping = actuators.longControlState == LongCtrlState.stopping
    if self.stop_cnt > 0:
      self.stop_cnt -= 1

    accel_val = interp( CS.lead_distance, [0, 30], [ -0.15, 0 ] )

    if CS.lead_distance > 20:
      self.stop_cnt = 0
    elif enabled and accel > accel_val and CS.out.vEgo < 10:
      if self.stop_cnt < 5:
        accel = accel_val
        gas_pressed = c.cruiseControl.override
        can_sends.extend( hyundaican.create_scc12(self.packer, accel, enabled, self.scc12_cnt, gas_pressed, stopping) )            
   
    self.accel = accel
    return can_sends    


  def update_ASCC(self, can_sends,  c, CS):
    cruise_accActive = CS.out.cruiseState.accActive
    if CS.cruise_set_mode == 5:
      cruise_accActive = CS.out.cruiseState.available
    pcm_cancel_cmd = c.cruiseControl.cancel
    if pcm_cancel_cmd:
      can_sends.append(create_clu11(self.packer, self.frame, CS.clu11, Buttons.CANCEL))
    elif cruise_accActive:
      if CS.out.cruiseState.standstill and not self.CP.opkrAutoResume:
        btn_signal = None
        self.stop_cnt = 100
      else:
        btn_signal = self.NC.update( c, CS, self.frame )
        if btn_signal != None:
          can_sends.append(create_clu11(self.packer, self.resume_cnt, CS.clu11, btn_signal ))
          self.resume_cnt += 1
        else:
          self.resume_cnt = 0

        #if self.CP.atompilotLongitudinalControl:
        #  if c.enabled and CS.out.cruiseState.accActive:
        #    if (self.frame % 2 == 0) and CS.cruise_set_mode == 2:
        #      self.update_scc12( can_sends, c, CS )
        #      self.scc12_cnt += 1
        #  else:
        #    self.accel = 0

    return  can_sends

  def update(self, c, CS ):
    enabled = c.enabled
    active = c.latActive
    actuators = c.actuators
    left_lane = c.hudControl.leftLaneVisible 
    right_lane = c.hudControl.rightLaneVisible 
    left_lane_warning = c.hudControl.leftLaneDepart 
    right_lane_warning = c.hudControl.rightLaneDepart
    
    self.gas = CS.out.gas

  
    # Steering Torque
    new_steer = int(round(actuators.steer * self.params.STEER_MAX))
    apply_steer = apply_std_steer_torque_limits(new_steer, self.apply_steer_last, CS.out.steeringTorque, self.params)
    #self.steer_rate_limited = new_steer != apply_steer

    if CS.engage_enable and not enabled:
      CS.engage_enable = False

    # disable when temp fault is active, or below LKA minimum speed
    # lkas_active = enabled and not CS.out.steerFaultTemporary and CS.out.vEgo >= self.CP.minSteerSpeed and CS.out.cruiseState.enabled
    lkas_active = enabled and active and not CS.out.steerFaultTemporary and  CS.out.vEgo >= self.CP.minSteerSpeed and CS.out.cruiseState.enabled


    if not lkas_active:
      apply_steer = 0
      self.stop_cnt = 0
      self.steer_timer_apply_torque = 0
    elif self.CP.smoothSteer.method == 1:
      apply_steer = self.smooth_steer( apply_steer, CS )
    elif abs(CS.out.steeringAngleDeg) > self.CP.maxSteeringAngleDeg:
      apply_steer = 0

    apply_steer = clip( apply_steer, -self.params.STEER_MAX, self.params.STEER_MAX )
    self.apply_steer_last = apply_steer
    sys_warning, sys_state = self.process_hud_alert( lkas_active, c, CS )


    if self.frame == 0: # initialize counts from last received count signals
      self.lkas11_cnt = CS.lkas11["CF_Lkas_MsgCount"] + 1
      self.scc12_cnt = CS.scc12["CR_VSM_Alive"] + 1  
  
    self.lkas11_cnt %= 0x10
    self.scc12_cnt %= 0x0F

    # tester present - w/ no response (keeps radar disabled)

    can_sends = []
    can_sends.append( create_lkas11(self.packer, self.lkas11_cnt, self.car_fingerprint, apply_steer, lkas_active,
                                   CS.lkas11, sys_warning, sys_state, enabled,
                                   left_lane, right_lane,
                                   left_lane_warning, right_lane_warning) )

    can_sends.append( create_mdps12(self.packer, self.frame, CS.mdps12) )

    self.update_ASCC( can_sends, c, CS )

      
    # 20 Hz LFA MFA message
    if self.frame % 5 == 0:
      self.update_debug( CS, c, apply_steer )
      if self.car_fingerprint in FEATURES["send_hda_mfa"]:
        can_sends.append( create_hda_mfc(self.packer, CS, c ) )
      elif self.car_fingerprint in FEATURES["send_lfa_mfa"]:
        can_sends.append( create_lfahda_mfc(self.packer, enabled) )
   

    new_actuators = actuators.copy()
    new_actuators.steer = apply_steer / self.params.STEER_MAX
    new_actuators.accel = self.accel
    new_actuators.gas = self.gas

    self.lkas11_cnt += 1
    self.frame += 1
    return new_actuators, can_sends
