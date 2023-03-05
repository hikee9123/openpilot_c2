import numpy as np
from common.realtime import sec_since_boot, DT_MDL
from common.numpy_fast import interp
from system.swaglog import cloudlog
from selfdrive.controls.lib.lateral_mpc_lib.lat_mpc import LateralMpc
from selfdrive.controls.lib.lateral_mpc_lib.lat_mpc import N as LAT_MPC_N
from selfdrive.controls.lib.drive_helpers import CONTROL_N,  MIN_SPEED
from selfdrive.controls.lib.lane_planner import LanePlanner, TRAJECTORY_SIZE
from selfdrive.controls.lib.desire_helper import DesireHelper
import cereal.messaging as messaging
from cereal import log


PATH_COST = 1.0
LATERAL_MOTION_COST = 0.11
LATERAL_ACCEL_COST = 0.0
LATERAL_JERK_COST = 0.05
# Extreme steering rate is unpleasant, even
# when it does not cause bad jerk.
# TODO this cost should be lowered when low
# speed lateral control is stable on all cars
STEERING_RATE_COST = 800.0


class LateralPlanner:
  def __init__(self, CP, end_to_end=True, wide_camera=False):

    self.LP = LanePlanner(wide_camera)
    self.DH = DesireHelper()

    # Vehicle model parameters used to calculate lateral movement of car
    self.factor1 = CP.wheelbase - CP.centerToFront
    self.factor2 = (CP.centerToFront * CP.mass) / (CP.wheelbase * CP.tireStiffnessRear)
    self.last_cloudlog_t = 0
    self.solution_invalid_cnt = 0

    self.path_xyz = np.zeros((TRAJECTORY_SIZE, 3))
    self.velocity_xyz = np.zeros((TRAJECTORY_SIZE, 3))  
    self.plan_yaw = np.zeros((TRAJECTORY_SIZE,))
    self.plan_yaw_rate = np.zeros((TRAJECTORY_SIZE,))
    self.t_idxs = np.arange(TRAJECTORY_SIZE)
    self.y_pts = np.zeros(TRAJECTORY_SIZE)

    self.lat_mpc = LateralMpc()
    self.reset_mpc(np.zeros(4))


    # atom
    self.cruise_buttons = 0
    self.time_enable = 0
    self.end_to_end = end_to_end    
    self.LP.end_to_end  = self.end_to_end

  def lanelines_check(self, sm):
    end_to_end = self.end_to_end
    steeringAngleDeg = sm['carState'].steeringAngleDeg
    cruiseSwState = sm['carState'].cruiseState.cruiseSwState
    accActive = sm['carState'].cruiseState.accActive
    enabled = sm['carState'].cruiseState.enabled

    if not enabled:
      self.time_enable = 100
    elif self.time_enable > 0:
      self.time_enable -= 1

    if self.cruise_buttons == cruiseSwState:  #   GAP_DIST = 3
      return end_to_end

    if accActive or self.time_enable:
      return end_to_end

    self.cruise_buttons = cruiseSwState
    if cruiseSwState == 3: #   GAP_DIST = 3
      if self.end_to_end:
        self.end_to_end = False
      else:
        self.end_to_end = True

    end_to_end = self.end_to_end
    return end_to_end
     

  def reset_mpc(self, x0=np.zeros(4)):
    self.x0 = x0
    self.lat_mpc.reset(x0=self.x0)

  def update(self, sm):
    # clip speed , lateral planning is not possible at 0 speed
    self.v_ego = max(MIN_SPEED, sm['carState'].vEgo)
    measured_curvature = sm['controlsState'].curvature

    # Parse model predictions
    md = sm['modelV2']
    self.LP.parse_model(md, sm)
    if len(md.position.x) == TRAJECTORY_SIZE and len(md.orientation.x) == TRAJECTORY_SIZE:
      self.path_xyz = np.column_stack([md.position.x, md.position.y, md.position.z])
      self.t_idxs = np.array(md.position.t)
      self.plan_yaw = np.array(md.orientation.z)
      self.plan_yaw_rate = np.array(md.orientationRate.z)
      self.velocity_xyz = np.column_stack([md.velocity.x, md.velocity.y, md.velocity.z])
      car_speed = np.linalg.norm(self.velocity_xyz, axis=1)
      self.v_plan = np.clip(car_speed, MIN_SPEED, np.inf)
      self.v_ego = self.v_plan[0]

    # Lane change logic
    lane_change_prob = self.LP.l_lane_change_prob + self.LP.r_lane_change_prob
    self.DH.update(sm, lane_change_prob)

    # Turn off lanes during lane change
    if self.DH.desire == log.LateralPlan.Desire.laneChangeRight or self.DH.desire == log.LateralPlan.Desire.laneChangeLeft:
      self.LP.lll_prob *= self.DH.lane_change_ll_prob
      self.LP.rll_prob *= self.DH.lane_change_ll_prob

    # Calculate final driving path and set MPC costs
    #if self.use_lanelines:
    self.LP.end_to_end   = self.lanelines_check(sm)
    if not self.LP.end_to_end:
      d_path_xyz = self.LP.get_d_path(self.v_ego, self.t_idxs, self.path_xyz)
      self.lat_mpc.set_weights(PATH_COST, LATERAL_MOTION_COST,
                             LATERAL_ACCEL_COST, LATERAL_JERK_COST,
                             STEERING_RATE_COST)
      y_pts = np.interp(self.v_ego * self.t_idxs[:LAT_MPC_N + 1], np.linalg.norm(d_path_xyz, axis=1), d_path_xyz[:, 1])
      heading_pts = np.interp(self.v_ego * self.t_idxs[:LAT_MPC_N + 1], np.linalg.norm(self.path_xyz, axis=1), self.plan_yaw)
      yaw_rate_pts = np.interp(self.v_ego * self.t_idxs[:LAT_MPC_N + 1], np.linalg.norm(self.path_xyz, axis=1), self.plan_yaw_rate)
      self.y_pts = y_pts
    else:
      self.path_xyz[:, 1] += self.LP.path_offset      
      self.lat_mpc.set_weights(PATH_COST, LATERAL_MOTION_COST,
                             LATERAL_ACCEL_COST, LATERAL_JERK_COST,
                             STEERING_RATE_COST)
      y_pts = self.path_xyz[:LAT_MPC_N+1, 1]
      heading_pts = self.plan_yaw[:LAT_MPC_N+1]
      yaw_rate_pts = self.plan_yaw_rate[:LAT_MPC_N+1]
      self.y_pts = y_pts

    assert len(y_pts) == LAT_MPC_N + 1
    assert len(heading_pts) == LAT_MPC_N + 1
    assert len(yaw_rate_pts) == LAT_MPC_N + 1
    #lateral_factor = max(0, self.factor1 - (self.factor2 * self.v_ego**2))
    lateral_factor = np.clip(self.factor1 - (self.factor2 * self.v_plan**2), 0.0, np.inf)
    #p = np.array([self.v_ego, lateral_factor])
    p = np.column_stack([self.v_plan, lateral_factor])
    self.lat_mpc.run(self.x0,
                     p,
                     y_pts,
                     heading_pts,
                     yaw_rate_pts)
    # init state for next iteration
    # mpc.u_sol is the desired second derivative of psi given x0 curv state.
    # with x0[3] = measured_yaw_rate, this would be the actual desired yaw rate.
    # instead, interpolate x_sol so that x0[3] is the desired yaw rate for lat_control.
    self.x0[3] = interp(DT_MDL, self.t_idxs[:LAT_MPC_N + 1], self.lat_mpc.x_sol[:, 3])

    #  Check for infeasible MPC solution
    mpc_nans = np.isnan(self.lat_mpc.x_sol[:, 3]).any()
    t = sec_since_boot()
    if mpc_nans or self.lat_mpc.solution_status != 0:
      self.reset_mpc()
      self.x0[3] = measured_curvature * self.v_ego
      if t > self.last_cloudlog_t + 5.0:
        self.last_cloudlog_t = t
        cloudlog.warning("Lateral mpc - nan: True")

    if self.lat_mpc.cost > 20000. or mpc_nans:
      self.solution_invalid_cnt += 1
    else:
      self.solution_invalid_cnt = 0

  def publish(self, sm, pm):
    plan_solution_valid = self.solution_invalid_cnt < 2
    plan_send = messaging.new_message('lateralPlan')
    plan_send.valid = sm.all_checks(service_list=['carState', 'controlsState', 'modelV2'])

    lateralPlan = plan_send.lateralPlan
    lateralPlan.modelMonoTime = sm.logMonoTime['modelV2']
    lateralPlan.laneWidth = float(self.LP.lane_width)
    lateralPlan.dPathPoints = self.y_pts.tolist()
    lateralPlan.psis = self.lat_mpc.x_sol[0:CONTROL_N, 2].tolist()

    lateralPlan.curvatures = (self.lat_mpc.x_sol[0:CONTROL_N, 3]/self.v_ego).tolist()
    lateralPlan.curvatureRates = [float(x/self.v_ego) for x in self.lat_mpc.u_sol[0:CONTROL_N - 1]] + [0.0]

    lateralPlan.lProb = float(self.LP.lll_prob)
    lateralPlan.rProb = float(self.LP.rll_prob)
    lateralPlan.dProb = float(self.LP.d_prob)

    lateralPlan.mpcSolutionValid = bool(plan_solution_valid)
    lateralPlan.solverExecutionTime = self.lat_mpc.solve_time

    lateralPlan.desire = self.DH.desire
    lateralPlan.useLaneLines = self.LP.end_to_end
    lateralPlan.laneChangeState = self.DH.lane_change_state
    lateralPlan.laneChangeDirection = self.DH.lane_change_direction


    pm.send('lateralPlan', plan_send)
