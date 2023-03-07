#!/usr/bin/env python3
from cereal import car
from panda import Panda
from common.params import Params
from common.conversions import Conversions as CV
from selfdrive.car.hyundai.tunes import update_lat_tune_param
from selfdrive.car.hyundai.values import CAR, DBC, EV_CAR, HYBRID_CAR, LEGACY_SAFETY_MODE_CAR, Buttons, CarControllerParams
from selfdrive.car.hyundai.radar_interface import RADAR_START_ADDR
from selfdrive.car import STD_CARGO_KG, scale_rot_inertia, scale_tire_stiffness, gen_empty_fingerprint, get_safety_config
from selfdrive.car.interfaces import CarInterfaceBase
from selfdrive.atom_conf import atom_params

ButtonType = car.CarState.ButtonEvent.Type
EventName = car.CarEvent.EventName

ATOMP = atom_params()

class CarInterface(CarInterfaceBase):
  @staticmethod
  def get_pid_accel_limits(CP, current_speed, cruise_speed):
    return CarControllerParams.ACCEL_MIN, CarControllerParams.ACCEL_MAX

  @staticmethod
  def get_normal_params( version, CP ):
    # version = 1. TURN,  2.lane control
    CP.laneParam.cameraOffsetAdj = float( Params().get("OpkrCameraOffsetAdj", encoding="utf8") )
    CP.laneParam.pathOffsetAdj = float( Params().get("OpkrPathOffsetAdj", encoding="utf8") )
    CP.laneParam.leftLaneOffset = float( Params().get("OpkrLeftLaneOffset", encoding="utf8") )
    CP.laneParam.rightLaneOffset = float( Params().get("OpkrRightLaneOffset", encoding="utf8") )
    CP.steerRatio = float( Params().get("OpkrSteerRatio", encoding="utf8") )

  @staticmethod
  def get_tunning_params( tune ):

    max_lat_accel = float( Params().get("TorqueMaxLatAccel", encoding="utf8") )
    tune.maxLateralAccel = max_lat_accel
    update_lat_tune_param( tune.lateralTuning, MAX_LAT_ACCEL=tune.maxLateralAccel )



  @staticmethod
  def get_params(candidate, fingerprint=gen_empty_fingerprint(), car_fw=[], experimental_long=False):  # pylint: disable=dangerous-default-value
    ret = CarInterfaceBase.get_std_params(candidate, fingerprint)

    Param = Params()
    ret.carName = "hyundai"
    ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.hyundai, 0)]
    ret.radarOffCan = False  # RADAR_START_ADDR not in fingerprint[1] or DBC[ret.carFingerprint]["radar"] is None
    # ret.communityFeature = True


    if (candidate in LEGACY_SAFETY_MODE_CAR):
      ret.atompilotLongitudinalControl = Param.get_bool("OpkratomLongitudinal")
    else:
      # WARNING: disabling radar also disables AEB (and we show the same warning on the instrument cluster as if you manually disabled AEB)
      ret.experimentalLongitudinalAvailable = candidate not in (LEGACY_SAFETY_MODE_CAR)
      ret.openpilotLongitudinalControl = experimental_long and (candidate not in (LEGACY_SAFETY_MODE_CAR))
    
    ret.pcmCruise = not ret.openpilotLongitudinalControl



    ret.opkrAutoResume = Param.get_bool("OpkrAutoResume")


    ret.maxSteeringAngleDeg = float( Param.get("OpkrMaxAngleLimit", encoding="utf8") )
    ret.smoothSteer.method = int( Param.get("OpkrSteerMethod", encoding="utf8") )   # 1
    ret.smoothSteer.maxSteeringAngle = float( Param.get("OpkrMaxSteeringAngle", encoding="utf8") )   # 90
    ret.smoothSteer.maxDriverAngleWait = float( Param.get("OpkrMaxDriverAngleWait", encoding="utf8") )  # 0.002
    ret.smoothSteer.maxSteerAngleWait = float( Param.get("OpkrMaxSteerAngleWait", encoding="utf8") )   # 0.001  # 10 sec
    ret.smoothSteer.driverAngleWait = float( Param.get("OpkrDriverAngleWait", encoding="utf8") )  #0.001 

    ret.laneParam.cameraOffsetAdj = float( Param.get("OpkrCameraOffsetAdj", encoding="utf8") )
    ret.laneParam.pathOffsetAdj = float( Param.get("OpkrPathOffsetAdj", encoding="utf8") )
    ret.laneParam.leftLaneOffset = float( Param.get("OpkrLeftLaneOffset", encoding="utf8") )
    ret.laneParam.rightLaneOffset = float( Param.get("OpkrRightLaneOffset", encoding="utf8") )

    # These cars have been put into dashcam only due to both a lack of users and test coverage.
    # These cars likely still work fine. Once a user confirms each car works and a test route is
    # added to selfdrive/car/tests/routes.py, we can remove it from this list.
    ret.dashcamOnly = candidate in {CAR.KIA_OPTIMA_H, CAR.ELANTRA_GT_I30}

    ret.steerActuatorDelay = 0.1  # Default delay
    ret.steerLimitTimer = 0.4
    tire_stiffness_factor = 1.

    ret.stoppingControl = False
    ret.vEgoStopping = 1.0

    ret.longitudinalTuning.kpV = [0.1]
    ret.longitudinalTuning.kiV = [0.0]
    ret.stopAccel = 0.0
    ret.wheelbase = 2.845


    ret.longitudinalActuatorDelayUpperBound = 1.0 # s

    if candidate in (CAR.GRANDEUR_HEV_19):
      ret.mass = 1675. + STD_CARGO_KG
      ret.wheelbase = 2.845
      ret.steerRatio = 16.5  #13.96   #12.5
      ret.steerActuatorDelay = 0.05   # 0.1, 0.05
      ret.minSteerSpeed = 0.3 * CV.KPH_TO_MS
      tire_stiffness_factor = 0.9
      ret.maxLateralAccel = 3.0


      ret.lateralTuning.pid.kf = 0.000005
      ret.lateralTuning.pid.kpBP, ret.lateralTuning.pid.kpV = [[0.], [0.25]]
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kiV = [[0.], [0.05]]

    elif candidate in (CAR.SANTA_FE, CAR.SANTA_FE_2022, CAR.SANTA_FE_HEV_2022, CAR.SANTA_FE_PHEV_2022):
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 3982. * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.766
      # Values from optimizer
      ret.steerRatio = 16.55  # 13.8 is spec end-to-end
      tire_stiffness_factor = 0.82
      ret.maxLateralAccel = 3.2
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[9., 22.], [9., 22.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.2, 0.35], [0.05, 0.09]]
    elif candidate in (CAR.SONATA, CAR.SONATA_HYBRID):
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 1513. + STD_CARGO_KG
      ret.wheelbase = 2.84
      ret.steerRatio = 13.27 * 1.15   # 15% higher at the center seems reasonable
      tire_stiffness_factor = 0.65
      ret.maxLateralAccel = 2.5
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.SONATA_LF:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 4497. * CV.LB_TO_KG
      ret.wheelbase = 2.804
      ret.steerRatio = 13.27 * 1.15   # 15% higher at the center seems reasonable
      ret.maxLateralAccel = 1.8
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.PALISADE:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 1999. + STD_CARGO_KG
      ret.wheelbase = 2.90
      ret.steerRatio = 15.6 * 1.15
      tire_stiffness_factor = 0.63
      ret.maxLateralAccel = 2.5
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.3], [0.05]]
    elif candidate in (CAR.ELANTRA, CAR.ELANTRA_GT_I30):
      ret.lateralTuning.pid.kf = 0.00006
      ret.mass = 1275. + STD_CARGO_KG
      ret.wheelbase = 2.7
      ret.steerRatio = 15.4            # 14 is Stock | Settled Params Learner values are steerRatio: 15.401566348670535
      tire_stiffness_factor = 0.385    # stiffnessFactor settled on 1.0081302973865127
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
      ret.minSteerSpeed = 32 * CV.MPH_TO_MS
    elif candidate == CAR.ELANTRA_2021:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = (2800. * CV.LB_TO_KG) + STD_CARGO_KG
      ret.wheelbase = 2.72
      ret.steerRatio = 12.9
      tire_stiffness_factor = 0.65
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.ELANTRA_HEV_2021:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = (3017. * CV.LB_TO_KG) + STD_CARGO_KG
      ret.wheelbase = 2.72
      ret.steerRatio = 12.9
      tire_stiffness_factor = 0.65
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.HYUNDAI_GENESIS:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 2060. + STD_CARGO_KG
      ret.wheelbase = 3.01
      ret.steerRatio = 16.5
      ret.lateralTuning.init('indi')
      ret.lateralTuning.indi.innerLoopGainBP = [0.]
      ret.lateralTuning.indi.innerLoopGainV = [3.5]
      ret.lateralTuning.indi.outerLoopGainBP = [0.]
      ret.lateralTuning.indi.outerLoopGainV = [2.0]
      ret.lateralTuning.indi.timeConstantBP = [0.]
      ret.lateralTuning.indi.timeConstantV = [1.4]
      ret.lateralTuning.indi.actuatorEffectivenessBP = [0.]
      ret.lateralTuning.indi.actuatorEffectivenessV = [2.3]
      ret.minSteerSpeed = 60 * CV.KPH_TO_MS
    elif candidate in (CAR.KONA, CAR.KONA_EV, CAR.KONA_HEV):
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = {CAR.KONA_EV: 1685., CAR.KONA_HEV: 1425.}.get(candidate, 1275.) + STD_CARGO_KG
      ret.wheelbase = 2.7
      ret.steerRatio = 13.73 * 1.15  # Spec
      tire_stiffness_factor = 0.385
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate in (CAR.IONIQ, CAR.IONIQ_EV_LTD, CAR.IONIQ_EV_2020, CAR.IONIQ_PHEV, CAR.IONIQ_HEV_2022):
      ret.lateralTuning.pid.kf = 0.00006
      ret.mass = 1490. + STD_CARGO_KG  # weight per hyundai site https://www.hyundaiusa.com/ioniq-electric/specifications.aspx
      ret.wheelbase = 2.7
      ret.steerRatio = 13.73  # Spec
      tire_stiffness_factor = 0.385
      ret.maxLateralAccel = 3.0
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
      if candidate not in (CAR.IONIQ_EV_2020, CAR.IONIQ_PHEV, CAR.IONIQ_HEV_2022):
        ret.minSteerSpeed = 32 * CV.MPH_TO_MS
    elif candidate == CAR.VELOSTER:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 3558. * CV.LB_TO_KG
      ret.wheelbase = 2.80
      ret.steerRatio = 13.75 * 1.15
      tire_stiffness_factor = 0.5
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.TUCSON_DIESEL_2019:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 3633. * CV.LB_TO_KG
      ret.wheelbase = 2.67
      ret.steerRatio = 14.00 * 1.15
      tire_stiffness_factor = 0.385
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]

    # Kia
    elif candidate == CAR.KIA_SORENTO:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 1985. + STD_CARGO_KG
      ret.wheelbase = 2.78
      ret.steerRatio = 14.4 * 1.1   # 10% higher at the center seems reasonable
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate in (CAR.KIA_NIRO_EV, CAR.KIA_NIRO_HEV, CAR.KIA_NIRO_HEV_2021):
      ret.lateralTuning.pid.kf = 0.00006
      ret.mass = 1737. + STD_CARGO_KG
      ret.wheelbase = 2.7
      ret.steerRatio = 13.9 if CAR.KIA_NIRO_HEV_2021 else 13.73  # Spec
      tire_stiffness_factor = 0.385
      ret.maxLateralAccel = 2.9
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
      if candidate == CAR.KIA_NIRO_HEV:
        ret.minSteerSpeed = 32 * CV.MPH_TO_MS
    elif candidate == CAR.KIA_SELTOS:
      ret.mass = 1337. + STD_CARGO_KG
      ret.wheelbase = 2.63
      ret.steerRatio = 14.56
      tire_stiffness_factor = 1
      ret.lateralTuning.init('indi')
      ret.lateralTuning.indi.innerLoopGainBP = [0.]
      ret.lateralTuning.indi.innerLoopGainV = [4.]
      ret.lateralTuning.indi.outerLoopGainBP = [0.]
      ret.lateralTuning.indi.outerLoopGainV = [3.]
      ret.lateralTuning.indi.timeConstantBP = [0.]
      ret.lateralTuning.indi.timeConstantV = [1.4]
      ret.lateralTuning.indi.actuatorEffectivenessBP = [0.]
      ret.lateralTuning.indi.actuatorEffectivenessV = [1.8]
    elif candidate in (CAR.KIA_OPTIMA, CAR.KIA_OPTIMA_H):
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 3558. * CV.LB_TO_KG
      ret.wheelbase = 2.80
      ret.steerRatio = 13.75
      tire_stiffness_factor = 0.5
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.KIA_STINGER:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 1825. + STD_CARGO_KG
      ret.wheelbase = 2.78
      ret.steerRatio = 14.4 * 1.15   # 15% higher at the center seems reasonable
      ret.maxLateralAccel = 2.4
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.KIA_FORTE:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 3558. * CV.LB_TO_KG
      ret.wheelbase = 2.80
      ret.steerRatio = 13.75
      tire_stiffness_factor = 0.5
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.KIA_CEED:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 1450. + STD_CARGO_KG
      ret.wheelbase = 2.65
      ret.steerRatio = 13.75
      tire_stiffness_factor = 0.5
      ret.lateralTuning.pid.kf = 0.00005
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]
    elif candidate == CAR.KIA_K5_2021:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 3228. * CV.LB_TO_KG
      ret.wheelbase = 2.85
      ret.steerRatio = 13.27  # 2021 Kia K5 Steering Ratio (all trims)
      tire_stiffness_factor = 0.5
      ret.maxLateralAccel = 2.1
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.25], [0.05]]

    # Genesis
    elif candidate == CAR.GENESIS_G70:
      ret.lateralTuning.init('indi')
      ret.lateralTuning.indi.innerLoopGainBP = [0.]
      ret.lateralTuning.indi.innerLoopGainV = [2.5]
      ret.lateralTuning.indi.outerLoopGainBP = [0.]
      ret.lateralTuning.indi.outerLoopGainV = [3.5]
      ret.lateralTuning.indi.timeConstantBP = [0.]
      ret.lateralTuning.indi.timeConstantV = [1.4]
      ret.lateralTuning.indi.actuatorEffectivenessBP = [0.]
      ret.lateralTuning.indi.actuatorEffectivenessV = [1.8]
      ret.steerActuatorDelay = 0.1
      ret.mass = 1640.0 + STD_CARGO_KG
      ret.wheelbase = 2.84
      ret.steerRatio = 13.56
    elif candidate == CAR.GENESIS_G70_2020:
      ret.lateralTuning.pid.kf = 0.
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.112], [0.004]]
      ret.mass = 3673.0 * CV.LB_TO_KG + STD_CARGO_KG
      ret.wheelbase = 2.83
      ret.steerRatio = 12.9
    elif candidate == CAR.GENESIS_G80:
      ret.lateralTuning.pid.kf = 0.00005
      ret.mass = 2060. + STD_CARGO_KG
      ret.wheelbase = 3.01
      ret.steerRatio = 16.5
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.16], [0.01]]
    elif candidate == CAR.GENESIS_G90:
      ret.mass = 2200
      ret.wheelbase = 3.15
      ret.steerRatio = 12.069
      ret.lateralTuning.pid.kiBP, ret.lateralTuning.pid.kpBP = [[0.], [0.]]
      ret.lateralTuning.pid.kpV, ret.lateralTuning.pid.kiV = [[0.16], [0.01]]

    CarInterface.get_tunning_params( ret )
    CarInterface.get_normal_params( 0, ret )

    CarInterface.live_tune( ret.atomTuning, True )

    ret.startingState = True
    ret.startAccel = 1.0
    # these cars require a special panda safety mode due to missing counters and checksums in the messages
    if candidate in LEGACY_SAFETY_MODE_CAR:
      ret.safetyConfigs = [get_safety_config(car.CarParams.SafetyModel.hyundaiLegacy)]

    # set appropriate safety param for gas signal
    if candidate in HYBRID_CAR:
      ret.safetyConfigs[0].safetyParam = Panda.FLAG_HYUNDAI_HYBRID_GAS
    elif candidate in EV_CAR:
      ret.safetyConfigs[0].safetyParam = Panda.FLAG_HYUNDAI_EV_GAS

    ret.centerToFront = ret.wheelbase * 0.4

    # TODO: get actual value, for now starting with reasonable value for
    # civic and scaling by mass and wheelbase
    ret.rotationalInertia = scale_rot_inertia(ret.mass, ret.wheelbase)

    # TODO: start from empirically derived lateral slip stiffness for the civic and scale by
    # mass and CG position, so all cars will have approximately similar dyn behaviors
    ret.tireStiffnessFront, ret.tireStiffnessRear = scale_tire_stiffness(ret.mass, ret.wheelbase, ret.centerToFront,
                                                                         tire_stiffness_factor=tire_stiffness_factor)

    ret.enableBsm = 0x58b in fingerprint[0]

    if ret.openpilotLongitudinalControl or ret.atompilotLongitudinalControl:
      ret.safetyConfigs[0].safetyParam |= Panda.FLAG_HYUNDAI_LONG

    return ret

  @staticmethod
  def live_tune( atomTuning, read=False):
    global ATOMP 
    Param = Params()
    OpkrLiveSteerRatio = int( Param.get('OpkrLiveSteerRatio') ) 
    if read and OpkrLiveSteerRatio == 2:
      ATOMP.read_tune()

    atomTuning.sRKPH     = ATOMP.sR_KPHV
    atomTuning.sRBPV     = ATOMP.sR_BPV
    atomTuning.sRsteerRatioV = ATOMP.sR_steerRatioV

    atomTuning.latFrictionV       = ATOMP.lat_friction
    atomTuning.latLatAccelFactorV = ATOMP.lat_latAccelFactor




  @staticmethod
  def init(CP, logcan, sendcan):
    pass

  def _update(self, c):
    ret = self.CS.update(self.cp, self.cp_cam, c)
    #ret.steeringRateLimited = self.CC.steer_rate_limited if self.CC is not None else False

    events = self.create_common_events(ret, pcm_enable=self.CS.CP.pcmCruise)

    if self.CS.brake_error:
      events.add(EventName.brakeUnavailable)


    # low speed steer alert hysteresis logic (only for cars with steer cut off above 10 m/s)
    if ret.vEgo < (self.CP.minSteerSpeed + 2.) and self.CP.minSteerSpeed > 10.:
      self.low_speed_alert = True
    if ret.vEgo > (self.CP.minSteerSpeed + 4.):
      self.low_speed_alert = False
    if self.low_speed_alert:
      events.add(EventName.belowSteerSpeed)
    elif self.CC.NC.event_navi_alert != None:
      events.add( self.CC.NC.event_navi_alert )
      self.CC.NC.event_navi_alert = None

    

    ret.events = events.to_msg()

    return ret

  def apply(self, c):
    ret = self.CC.update( c, self.CS )

    return ret
