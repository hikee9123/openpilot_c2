#!/usr/bin/env python3
from enum import Enum
from cereal import car
from common.params import Params






class TunType(Enum):
  LAT_PID = 0
  LAT_INDI = 1
  LAT_LQR = 2
  LAT_TOROUE = 3



class LongTunes(Enum):
  PEDAL = 0
  TSS2 = 1
  TSS = 2

class LatTunes(Enum):
  INDI_PRIUS = 0
  LQR_RAV4 = 1
  PID_A = 2
  PID_B = 3
  PID_C = 4
  PID_D = 5
  PID_E = 6
  PID_F = 7
  PID_G = 8
  PID_I = 9
  PID_H = 10
  PID_J = 11
  PID_K = 12
  PID_L = 13
  PID_M = 14
  PID_N = 15
  TORQUE = 16
  HYBRID = 17
  MULTI = 18






###### LONG ######
def set_long_tune(tune, name):
  # Improved longitudinal tune
  if name == LongTunes.TSS2 or name == LongTunes.PEDAL:
    tune.deadzoneBP = [0., 8.05]
    tune.deadzoneV = [.0, .14]
    tune.kpBP = [0., 5., 20.]
    tune.kpV = [1.3, 1.0, 0.7]
    tune.kiBP = [0., 5., 12., 20., 27.]
    tune.kiV = [.35, .23, .20, .17, .1]
  # Default longitudinal tune
  elif name == LongTunes.TSS:
    tune.deadzoneBP = [0., 9.]
    tune.deadzoneV = [0., .15]
    tune.kpBP = [0., 5., 35.]
    tune.kiBP = [0., 35.]
    tune.kpV = [3.6, 2.4, 1.5]
    tune.kiV = [0.54, 0.36]
  else:
    raise NotImplementedError('This longitudinal tune does not exist')

def update_lat_tune_param(tune, MAX_LAT_ACCEL=2.5, steering_angle_deadzone_deg=0.0):
  params = Params()

  OpkrLateralControlMethod = int( params.get("OpkrLateralControlMethod", encoding="utf8") )
  method = TunType( OpkrLateralControlMethod )
  print( 'OpkrLateralControlMethod = {}'.format( method ))
  if method ==  TunType.LAT_PID:
      Kp = float( params.get("PidKp", encoding="utf8") )
      Ki = float( params.get("PidKi", encoding="utf8") )
      Kf = float( params.get("PidKf", encoding="utf8") )

      tune.init('pid')
      tune.pid.kiBP = [0.0]
      tune.pid.kpBP = [0.0]
      tune.pid.kpV = [Kp]
      tune.pid.kiV = [Ki]
      tune.pid.kf = Kf
  elif method ==  TunType.LAT_LQR:
      scale = float( params.get("LqrScale", encoding="utf8") )
      Ki = float( params.get("LqrKi", encoding="utf8") )
      dcGain = float( params.get("LqrDcGain", encoding="utf8") )    

      tune.init('lqr')
      tune.lqr.scale = scale  #1900     #1700.0
      tune.lqr.ki = Ki #0.01      #0.01
      tune.lqr.dcGain = dcGain # 0.0027  #0.0027
      # 호야  1500, 0.015, 0.0027
      #  1700, 0.01, 0.0029
      #  2000, 0.01, 0.003
      # toyota  1500, 0.05,   0.002237852961363602

      tune.lqr.a = [0., 1., -0.22619643, 1.21822268]
      tune.lqr.b = [-1.92006585e-04, 3.95603032e-05]
      tune.lqr.c = [1., 0.]
      tune.lqr.k = [-110.73572306, 451.22718255]
      tune.lqr.l = [0.3233671, 0.3185757]
  elif method ==  TunType.LAT_TOROUE:
      FRICTION      = float( params.get("TorqueFriction", encoding="utf8") )
      Kp            = float( params.get("TorqueKp", encoding="utf8") )
      Ki            = float( params.get("TorqueKi", encoding="utf8") )
      Kf            = float( params.get("TorqueKf", encoding="utf8") )
      UseAngle      = True  #params.getBool("TorqueUseAngle", encoding="utf8")
      steering_angle_deadzone_deg  = float( params.get("Torquedeadzone", encoding="utf8") )

      tune.init('torque')
      tune.torque.useSteeringAngle = UseAngle  #  False
      tune.torque.kp = Kp #      1.0
      tune.torque.kf = Kf #      1.0
      tune.torque.ki = Ki #      0.1
      tune.torque.friction = FRICTION
      tune.torque.latAccelFactor = MAX_LAT_ACCEL      
      tune.torque.latAccelOffset = 0.0      
      tune.torque.steeringAngleDeadzoneDeg = steering_angle_deadzone_deg

  else:
      print( 'OpkrLateralControlMethod = tune.init(lqr)')
      tune.init('lqr')
      tune.lqr.scale = 1900     #1700.0
      tune.lqr.ki = 0.01      #0.01
      tune.lqr.dcGain =  0.0027  #0.0027
      # 호야  1500, 0.015, 0.0027
      #  1700, 0.01, 0.0029
      #  2000, 0.01, 0.003
      # toyota  1500, 0.05,   0.002237852961363602

      tune.lqr.a = [0., 1., -0.22619643, 1.21822268]
      tune.lqr.b = [-1.92006585e-04, 3.95603032e-05]
      tune.lqr.c = [1., 0.]
      tune.lqr.k = [-110.73572306, 451.22718255]
      tune.lqr.l = [0.3233671, 0.3185757]
  
  return  method
