#include "selfdrive/ui/ui.h"

#include <cassert>
#include <cmath>

#include <QtConcurrent>

#include "common/transformations/orientation.hpp"
#include "selfdrive/common/params.h"
#include "selfdrive/common/swaglog.h"
#include "selfdrive/common/util.h"
#include "selfdrive/common/watchdog.h"
#include "system/hardware/hw.h"

#define BACKLIGHT_DT 0.05
#define BACKLIGHT_TS 10.00
#define BACKLIGHT_OFFROAD 50

// Projects a point in car to space to the corresponding point in full frame
// image space.
static bool calib_frame_to_full_frame(const UIState *s, float in_x, float in_y, float in_z, QPointF *out) {
  const float margin = 500.0f;
  const QRectF clip_region{-margin, -margin, s->fb_w + 2 * margin, s->fb_h + 2 * margin};

  const vec3 pt = (vec3){{in_x, in_y, in_z}};
  const vec3 Ep = matvecmul3(s->scene.view_from_calib, pt);
  const vec3 KEp = matvecmul3(s->wide_camera ? ecam_intrinsic_matrix : fcam_intrinsic_matrix, Ep);

  // Project.
  QPointF point = s->car_space_transform.map(QPointF{KEp.v[0] / KEp.v[2], KEp.v[1] / KEp.v[2]});
  if (clip_region.contains(point)) {
    *out = point;
    return true;
  }
  return false;
}

static int get_path_length_idx(const cereal::XYZTData::Reader &line, const float path_height) {
  const auto line_x = line.getX();
  int max_idx = 0;
  for (int i = 1; i < TRAJECTORY_SIZE && line_x[i] <= path_height; ++i) {
    max_idx = i;
  }
  return max_idx;
}

static void update_leads(UIState *s, const cereal::RadarState::Reader &radar_state, const cereal::XYZTData::Reader &line) {
  for (int i = 0; i < 2; ++i) {
    auto lead_data = (i == 0) ? radar_state.getLeadOne() : radar_state.getLeadTwo();
    if (lead_data.getStatus()) {
      float z = line.getZ()[get_path_length_idx(line, lead_data.getDRel())];
      calib_frame_to_full_frame(s, lead_data.getDRel(), -lead_data.getYRel(), z + 1.22, &s->scene.lead_vertices[i]);
    }
  }
}

static void update_line_data(const UIState *s, const cereal::XYZTData::Reader &line,
                             float y_off, float z_off, line_vertices_data *pvd, int max_idx, bool allow_invert=true) {
  const auto line_x = line.getX(), line_y = line.getY(), line_z = line.getZ();

  std::vector<QPointF> left_points, right_points;
  for (int i = 0; i <= max_idx; i++) {
    QPointF left, right;
    bool l = calib_frame_to_full_frame(s, line_x[i], line_y[i] - y_off, line_z[i] + z_off, &left);
    bool r = calib_frame_to_full_frame(s, line_x[i], line_y[i] + y_off, line_z[i] + z_off, &right);
    if (l && r) {
      // For wider lines the drawn polygon will "invert" when going over a hill and cause artifacts
      if (!allow_invert && left_points.size() && left.y() > left_points.back().y()) {
        continue;
      }
      left_points.push_back(left);
      right_points.push_back(right);
    }
  }

  pvd->cnt = 2 * left_points.size();
  assert(left_points.size() == right_points.size());
  assert(pvd->cnt <= std::size(pvd->v));

  for (int left_idx = 0; left_idx < left_points.size(); left_idx++){
    int right_idx = 2 * left_points.size() - left_idx - 1;
    pvd->v[left_idx] = left_points[left_idx];
    pvd->v[right_idx] = right_points[left_idx];
  }
}



static void update_blindspot_data(const UIState *s, int lr, const cereal::XYZTData::Reader &line,
                             float y_off,  line_vertices_data *pvd, int max_idx ) {
  float  y_off1, y_off2;

  float z_off_left = 0;  //def:0.0
  float z_off_right = 0;

  if( lr == 0 ) // left
  {
    y_off1 = y_off;
    y_off2 = 0;
  }
  else  // left
  {
      y_off1 = 0;
      y_off2 = y_off;  
  }


  const auto line_x = line.getX(), line_y = line.getY(), line_z = line.getZ();
  QPointF *v = &pvd->v[0]; // *v = &pvd->v[0];
  for (int i = 0; i <= max_idx; i++) {
    v += calib_frame_to_full_frame(s, line_x[i], line_y[i] - y_off1, line_z[i] + z_off_left, v);
  }
  for (int i = max_idx; i >= 0; i--) {
    v += calib_frame_to_full_frame(s, line_x[i], line_y[i] + y_off2, line_z[i] + z_off_right, v);
  }

  pvd->cnt = v - pvd->v;
  assert(pvd->cnt <= std::size(pvd->v));

}

/*
static void update_model(UIState *s, 
                         const cereal::ModelDataV2::Reader &model
                         const cereal::UiPlan::Reader &plan) {
  UIScene &scene = s->scene;
  auto model_position = model.getPosition();
  float max_distance = std::clamp(model_position.getX()[TRAJECTORY_SIZE - 1],
                                  MIN_DRAW_DISTANCE, MAX_DRAW_DISTANCE);

  // update lane lines
  const auto lane_lines = model.getLaneLines();
  const auto lane_line_probs = model.getLaneLineProbs();
  int max_idx = get_path_length_idx(lane_lines[0], max_distance);
  for (int i = 0; i < std::size(scene.lane_line_vertices); i++) {
    scene.lane_line_probs[i] = lane_line_probs[i];
    update_line_data(s, lane_lines[i], 0.025 * scene.lane_line_probs[i], 0, &scene.lane_line_vertices[i], max_idx);  
  }


  // lane barriers for blind spot
  int max_distance_barrier =  40;
  int max_idx_barrier = std::min(max_idx, get_path_length_idx(lane_lines[0], max_distance_barrier));
  update_blindspot_data(s, 0, lane_lines[1], 2.5, &scene.lane_blindspot_vertices[0], max_idx_barrier);
  update_blindspot_data(s, 1, lane_lines[2], 2.5, &scene.lane_blindspot_vertices[1], max_idx_barrier);

  // update road edges
  const auto road_edges = model.getRoadEdges();
  const auto road_edge_stds = model.getRoadEdgeStds();
  for (int i = 0; i < std::size(scene.road_edge_vertices); i++) {
    scene.road_edge_stds[i] = road_edge_stds[i];
    update_line_data(s, road_edges[i], 0.025, 0, &scene.road_edge_vertices[i], max_idx);
  }

  // update path
  auto lead_one = (*s->sm)["radarState"].getRadarState().getLeadOne();
  if (lead_one.getStatus()) {
    const float lead_d = lead_one.getDRel() * 2.;
    max_distance = std::clamp((float)(lead_d - fmin(lead_d * 0.35, 5.)), 0.0f, max_distance);
  }
  max_idx = get_path_length_idx(model_position, max_distance);
  update_line_data(s, model_position, scene.end_to_end ? 0.9 : 0.5, 1.22, &scene.track_vertices, max_idx, false);


  // update stop lines
  scene.stopLine.used = true;
  if ( scene.stopLine.used ) {
    const auto stop_line = model.getStopLine();
    scene.stopLine.Prob = stop_line.getProb();
    if ( scene.stopLine.Prob > .1) {
      scene.stopLine.x = stop_line.getX();
      scene.stopLine.y = stop_line.getY();
      scene.stopLine.z = stop_line.getZ();
    }
    else
    {
      scene.stopLine.x = 0;
    }
  }  
}
*/

void update_model(UIState *s, 
                  const cereal::ModelDataV2::Reader &model,
                  const cereal::UiPlan::Reader &plan) {
  UIScene &scene = s->scene;
  auto model_position = model.getPosition();
  float max_distance1 = std::clamp(model_position.getX()[TRAJECTORY_SIZE - 1],
                                  MIN_DRAW_DISTANCE, MAX_DRAW_DISTANCE);

  auto plan_position = plan.getPosition();
  if (plan_position.getX().size() < TRAJECTORY_SIZE){
    plan_position = model.getPosition();
  }
  float max_distance2 = std::clamp(plan_position.getX()[TRAJECTORY_SIZE - 1],
                                  MIN_DRAW_DISTANCE, MAX_DRAW_DISTANCE);

  float max_distance = max_distance1;
  if( scene.end_to_end )
  {
     max_distance = max_distance2;
  }

  // update lane lines
  const auto lane_lines = model.getLaneLines();
  const auto lane_line_probs = model.getLaneLineProbs();
  int max_idx = get_path_length_idx(lane_lines[0], max_distance);
  for (int i = 0; i < std::size(scene.lane_line_vertices); i++) {
    scene.lane_line_probs[i] = lane_line_probs[i];
    update_line_data(s, lane_lines[i], 0.025 * scene.lane_line_probs[i], 0, &scene.lane_line_vertices[i], max_idx);
  }

  // update road edges
  const auto road_edges = model.getRoadEdges();
  const auto road_edge_stds = model.getRoadEdgeStds();
  for (int i = 0; i < std::size(scene.road_edge_vertices); i++) {
    scene.road_edge_stds[i] = road_edge_stds[i];
    update_line_data(s, road_edges[i], 0.025, 0, &scene.road_edge_vertices[i], max_idx);
  }


  // update path
  auto lead_one = (*s->sm)["radarState"].getRadarState().getLeadOne();
  if (lead_one.getStatus()) {
    const float lead_d = lead_one.getDRel() * 2.;
    max_distance = std::clamp((float)(lead_d - fmin(lead_d * 0.35, 10.)), 0.0f, max_distance);
  }

  if( scene.end_to_end )
  {
    max_idx = get_path_length_idx(plan_position, max_distance);
    update_line_data(s, plan_position, 0.9, 1.22, &scene.track_vertices, max_idx, false);
  }
  else
  {
    max_idx = get_path_length_idx(model_position, max_distance);
    update_line_data(s, model_position, 0.5, 1.22, &scene.track_vertices, max_idx, false);
  }





  // lane barriers for blind spot
  int max_distance_barrier =  40;
  int max_idx_barrier = std::min(max_idx, get_path_length_idx(lane_lines[0], max_distance_barrier));
  update_blindspot_data(s, 0, lane_lines[1], 2.5, &scene.lane_blindspot_vertices[0], max_idx_barrier);
  update_blindspot_data(s, 1, lane_lines[2], 2.5, &scene.lane_blindspot_vertices[1], max_idx_barrier);

  // update stop lines
  scene.stopLine.used = true;
  if ( scene.stopLine.used ) {
    const auto stop_line = model.getStopLine();
    scene.stopLine.Prob = stop_line.getProb();
    if ( scene.stopLine.Prob > .1) {
      scene.stopLine.x = stop_line.getX();
      scene.stopLine.y = stop_line.getY();
      scene.stopLine.z = stop_line.getZ();
    }
    else
    {
      scene.stopLine.x = 0;
    }
  }  
}


void update_dmonitoring(UIState *s, const cereal::DriverState::Reader &driverstate, float dm_fade_state ) {
  UIScene &scene = s->scene;
  const auto driver_orient = driverstate.getFaceOrientation();
  for (int i = 0; i < std::size(scene.driver_pose_vals); i++) {
    float v_this = (i == 0 ? (driver_orient[i] < 0 ? 0.7 : 0.9) : 0.4) * driver_orient[i];
    scene.driver_pose_diff[i] = fabs(scene.driver_pose_vals[i] - v_this);
    scene.driver_pose_vals[i] = 0.8 * v_this + (1 - 0.8) * scene.driver_pose_vals[i];
    scene.driver_pose_sins[i] = sinf(scene.driver_pose_vals[i]*(1.0-dm_fade_state));
    scene.driver_pose_coss[i] = cosf(scene.driver_pose_vals[i]*(1.0-dm_fade_state));
  }

  const mat3 r_xyz = (mat3){{
    scene.driver_pose_coss[1]*scene.driver_pose_coss[2],
    scene.driver_pose_coss[1]*scene.driver_pose_sins[2],
    -scene.driver_pose_sins[1],

    -scene.driver_pose_sins[0]*scene.driver_pose_sins[1]*scene.driver_pose_coss[2] - scene.driver_pose_coss[0]*scene.driver_pose_sins[2],
    -scene.driver_pose_sins[0]*scene.driver_pose_sins[1]*scene.driver_pose_sins[2] + scene.driver_pose_coss[0]*scene.driver_pose_coss[2],
    -scene.driver_pose_sins[0]*scene.driver_pose_coss[1],

    scene.driver_pose_coss[0]*scene.driver_pose_sins[1]*scene.driver_pose_coss[2] - scene.driver_pose_sins[0]*scene.driver_pose_sins[2],
    scene.driver_pose_coss[0]*scene.driver_pose_sins[1]*scene.driver_pose_sins[2] + scene.driver_pose_sins[0]*scene.driver_pose_coss[2],
    scene.driver_pose_coss[0]*scene.driver_pose_coss[1],
  }};

  // transform vertices
  for (int kpi = 0; kpi < std::size(default_face_kpts_3d); kpi++) {
    vec3 kpt_this = default_face_kpts_3d[kpi];
    kpt_this = matvecmul3(r_xyz, kpt_this);
    scene.face_kpts_draw[kpi] = (vec3){{(float)kpt_this.v[0], (float)kpt_this.v[1], (float)(kpt_this.v[2] * (1.0-dm_fade_state) + 8 * dm_fade_state)}};
  }
}

static void update_sockets(UIState *s) {
  s->sm->update(0);
}

static void update_state(UIState *s) {
  SubMaster &sm = *(s->sm);
  UIScene &scene = s->scene;

  if (sm.updated("liveCalibration")) {
    auto rpy_list = sm["liveCalibration"].getLiveCalibration().getRpyCalib();
    Eigen::Vector3d rpy;
    rpy << rpy_list[0], rpy_list[1], rpy_list[2];
    Eigen::Matrix3d device_from_calib = euler2rot(rpy);
    Eigen::Matrix3d view_from_device;
    view_from_device << 0,1,0,
                        0,0,1,
                        1,0,0;
    Eigen::Matrix3d view_from_calib = view_from_device * device_from_calib;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        scene.view_from_calib.v[i*3 + j] = view_from_calib(i,j);
      }
    }
  }
  if (s->worldObjectsVisible()) {
    if (sm.updated("modelV2")) {
      update_model(s, sm["modelV2"].getModelV2(), sm["uiPlan"].getUiPlan());
    }
    if (sm.updated("radarState") && sm.rcv_frame("modelV2") > s->scene.started_frame) {
      update_leads(s, sm["radarState"].getRadarState(), sm["modelV2"].getModelV2().getPosition());
    }
  }
  if (sm.updated("pandaStates")) {
    auto pandaStates = sm["pandaStates"].getPandaStates();
    if (pandaStates.size() > 0) {
      scene.pandaType = pandaStates[0].getPandaType();

      if (scene.pandaType != cereal::PandaState::PandaType::UNKNOWN) {
        scene.ignition = false;
        for (const auto& pandaState : pandaStates) {
          scene.ignition |= pandaState.getIgnitionLine() || pandaState.getIgnitionCan();
        }
      }
    }
  } else if ((s->sm->frame - s->sm->rcv_frame("pandaStates")) > 5*UI_FREQ) {
    scene.pandaType = cereal::PandaState::PandaType::UNKNOWN;
  }
  if (sm.updated("carParams")) {
    scene.car_params = sm["carParams"].getCarParams();


    scene.longitudinal_control = scene.car_params.getOpenpilotLongitudinalControl();
    scene.longitudinal_control |= scene.car_params.getAtompilotLongitudinalControl();

    //printf("carParams  %d\n", scene.longitudinal_control );  
  }


  float  gradient[2];
  gradient[0] = scene.scr.accel_prob[0];
  gradient[1] = scene.scr.accel_prob[1];
  if ( sm.updated("sensorEvents")) {
    for (auto sensor : sm["sensorEvents"].getSensorEvents()) {
      if (sensor.which() == cereal::SensorEventData::ACCELERATION) {
        auto accel = sensor.getAcceleration().getV();
        if (accel.totalSize().wordCount) { // TODO: sometimes empty lists are received. Figure out why
          scene.accel_sensor = accel[2];

          gradient[0] = atan(accel[2]/accel[0]) * (180 / M_PI); // back and forth
          gradient[1] = atan(accel[1]/accel[0]) * (180 / M_PI); // right and left 

          scene.scr.accel_prob[0]  = gradient[0];
          scene.scr.accel_prob[1]  = gradient[1];
        }
      } else if (sensor.which() == cereal::SensorEventData::GYRO_UNCALIBRATED) {
        auto gyro = sensor.getGyroUncalibrated().getV();
        if (gyro.totalSize().wordCount) {
          scene.gyro_sensor = gyro[1];
        }
      }
    }
  }
  if (!Hardware::TICI() && sm.updated("roadCameraState")) {
    auto camera_state = sm["roadCameraState"].getRoadCameraState();

    float max_lines = Hardware::EON() ? 5408 : 1904;
    float max_gain = Hardware::EON() ? 1.0: 10.0;
    float max_ev = max_lines * max_gain;

    float ev = camera_state.getGain() * float(camera_state.getIntegLines());

    scene.light_sensor = std::clamp<float>(1.0 - (ev / max_ev), 0.0, 1.0);
  } else if (Hardware::TICI() && sm.updated("wideRoadCameraState")) {
    auto camera_state = sm["wideRoadCameraState"].getWideRoadCameraState();

    float max_lines = 1618;
    float max_gain = 10.0;
    float max_ev = max_lines * max_gain / 6;

    float ev = camera_state.getGain() * float(camera_state.getIntegLines());

    scene.light_sensor = std::clamp<float>(1.0 - (ev / max_ev), 0.0, 1.0);
  }
  //scene.started = sm["deviceState"].getDeviceState().getStarted() && scene.ignition;



  if( scene.IsOpenpilotViewEnabled )
  {
    if ( scene.pandaType != cereal::PandaState::PandaType::UNKNOWN) 
    {
      scene.IsOpenpilotViewEnabled = false;
      char value = '0';
      Params().put("IsOpenpilotViewEnabled", &value, 1);
    }
      

     scene.started = sm["deviceState"].getDeviceState().getStarted();
  }
  else
  {
     scene.started = sm["deviceState"].getDeviceState().getStarted() && scene.ignition;
  }
    


  
   if (sm.updated("gpsLocationExternal")) {
    scene.gpsLocationExternal = sm["gpsLocationExternal"].getGpsLocationExternal();
   }

   if (sm.updated("deviceState")) {
    scene.deviceState = sm["deviceState"].getDeviceState();
   }
    
   if (scene.started && sm.updated("controlsState")) {
    scene.controls_state = sm["controlsState"].getControlsState();
// debug Message
    scene.alert.alertTextMsg1 = scene.controls_state.getAlertTextMsg1();
    scene.alert.alertTextMsg2 = scene.controls_state.getAlertTextMsg2();
    scene.alert.alertTextMsg3 = scene.controls_state.getAlertTextMsg3();
   } 
   if (sm.updated("carState")) {
    scene.car_state = sm["carState"].getCarState();

    auto cruiseState = scene.car_state.getCruiseState();
    scene.scr.awake = cruiseState.getCruiseSwState();
    scene.scr.enginrpm =  scene.car_state.getEngineRpm();


    scene.scr.leftblindspot = scene.car_state.getLeftBlindspot();
    scene.scr.rightblindspot = scene.car_state.getRightBlindspot();    
   } 
   
   if( sm.updated("liveNaviData"))
   {
     scene.liveNaviData = sm["liveNaviData"].getLiveNaviData();
     scene.scr.map_is_running = scene.liveNaviData.getMapEnable();
   } 

   if( sm.updated("liveParameters") )
   {
      scene.liveParameters = sm["liveParameters"].getLiveParameters();
   }

   if (sm.updated("lateralPlan"))
   {
    scene.lateralPlan = sm["lateralPlan"].getLateralPlan();
   } 

  if ( sm.updated("updateEvents")) {

    scene.update_data = sm["updateEvents"].getUpdateEvents();

    //int nCmd = scene.update_data.getCommand();
    //printf("updateEvents cmd = %d", nCmd );
  }



}

void ui_update_params(UIState *s) {
  s->scene.is_metric = Params().getBool("IsMetric");
  s->scene.IsOpenpilotViewEnabled = Params().getBool("IsOpenpilotViewEnabled");
}

void UIState::updateStatus() {
  if (scene.started && sm->updated("controlsState")) {
    auto controls_state = (*sm)["controlsState"].getControlsState();
    auto alert_status = controls_state.getAlertStatus();
    auto state = controls_state.getState();
    if (alert_status == cereal::ControlsState::AlertStatus::USER_PROMPT) {
      status = STATUS_WARNING;
    } else if (alert_status == cereal::ControlsState::AlertStatus::CRITICAL) {
      status = STATUS_ALERT;
    } else if (state == cereal::ControlsState::OpenpilotState::PRE_ENABLED || state == cereal::ControlsState::OpenpilotState::OVERRIDING) {
      status = STATUS_OVERRIDE;
    } else {
      status = controls_state.getEnabled() ? STATUS_ENGAGED : STATUS_DISENGAGED;
    }
  }

  // Handle onroad/offroad transition
  if (scene.started != started_prev || sm->frame == 1) {
    if (scene.started) {
      status = STATUS_DISENGAGED;
      scene.started_frame = sm->frame;
      scene.end_to_end = Params().getBool("EndToEndToggle");
      wide_camera = Hardware::TICI() ? Params().getBool("EnableWideCamera") : false;
    }
    started_prev = scene.started;
    emit offroadTransition(!scene.started);
  
  } else  {
     scene.end_to_end = scene.lateralPlan.getUseLaneLines();
  }
}

UIState::UIState(QObject *parent) : QObject(parent) {
  sm = std::make_unique<SubMaster, const std::initializer_list<const char *>>({
    "modelV2", "controlsState", "liveCalibration", "radarState", "deviceState", "roadCameraState",
    "pandaStates", "carParams", "driverMonitoringState", "sensorEvents", "carState", "liveLocationKalman",
    "wideRoadCameraState",
    "liveNaviData", "gpsLocationExternal", "lateralPlan", "liveParameters","updateEvents","longitudinalPlan",
    "uiPlan",
  });

  Params params;
  wide_camera = Hardware::TICI() ? params.getBool("EnableWideCamera") : false;
  prime_type = std::atoi(params.get("PrimeType").c_str());
  language = QString::fromStdString(params.get("LanguageSetting"));

  // update timer
  timer = new QTimer(this);
  QObject::connect(timer, &QTimer::timeout, this, &UIState::update);
  timer->start(1000 / UI_FREQ);
}

void UIState::update() {
  update_sockets(this);
  update_state(this);
  updateStatus();

  if (sm->frame % UI_FREQ == 0) {
    watchdog_kick();
  }
  emit uiUpdate(*this);
}

Device::Device(QObject *parent) : brightness_filter(BACKLIGHT_OFFROAD, BACKLIGHT_TS, BACKLIGHT_DT), QObject(parent) {
  setAwake(true);
  resetInteractiveTimout();

  QObject::connect(uiState(), &UIState::uiUpdate, this, &Device::update);
}

void Device::update(const UIState &s) {
  updateBrightness(s);
  updateWakefulness(s);

  // TODO: remove from UIState and use signals
  uiState()->awake = awake;
}

void Device::setAwake(bool on) {
  if (on != awake) {
    awake = on;

    // atom
    UIScene  &scene =  uiState()->scene; 
    if( scene.ignition || !scene.scr.autoScreenOff )
    {
    Hardware::set_display_power(awake);
    LOGD("setting display power %d", awake);
    emit displayPowerChanged(awake);
  }
}
}

void Device::resetInteractiveTimout() {
  interactive_timeout = (ignition_on ? 10 : 30) * UI_FREQ;

  UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
  scene.scr.nTime = scene.scr.autoScreenOff * 60 * UI_FREQ;
}

void Device::updateBrightness(const UIState &s) {
  float clipped_brightness = BACKLIGHT_OFFROAD;
  if (s.scene.started) {
    // Scale to 0% to 100%
    clipped_brightness = 100.0 * s.scene.light_sensor;

    // CIE 1931 - https://www.photonstophotos.net/GeneralTopics/Exposure/Psychometric_Lightness_and_Gamma.htm
    if (clipped_brightness <= 8) {
      clipped_brightness = (clipped_brightness / 903.3);
    } else {
      clipped_brightness = std::pow((clipped_brightness + 16.0) / 116.0, 3.0);
    }

    // Scale back to 10% to 100%
    clipped_brightness = std::clamp(100.0f * clipped_brightness, 10.0f, 100.0f);
  }

  int brightness = brightness_filter.update(clipped_brightness);
  if (!awake) {
    brightness = 0;
  } else if (s.scene.started && s.scene.scr.nTime <= 0 && s.scene.scr.autoScreenOff != 0) {
    brightness = s.scene.scr.brightness_off * 0.01 * brightness;
  }

  if (brightness != last_brightness) {
    if (!brightness_future.isRunning()) {
      brightness_future = QtConcurrent::run(Hardware::set_brightness, brightness);
      last_brightness = brightness;
    }
  }
}

bool Device::motionTriggered(const UIState &s) {
  static float accel_prev = 0;
  static float gyro_prev = 0;

  bool accel_trigger = abs(s.scene.accel_sensor - accel_prev) > 0.2;
  bool gyro_trigger = abs(s.scene.gyro_sensor - gyro_prev) > 0.15;

  gyro_prev = s.scene.gyro_sensor;
  accel_prev = (accel_prev * (accel_samples - 1) + s.scene.accel_sensor) / accel_samples;

  return (!awake && accel_trigger && gyro_trigger);
}

void Device::updateWakefulness(const UIState &s) {
  bool ignition_just_turned_off = !s.scene.ignition && ignition_on;
  ignition_on = s.scene.ignition;

  if (ignition_just_turned_off || motionTriggered(s)) {
    resetInteractiveTimout();
  } else if (interactive_timeout > 0 && --interactive_timeout == 0) {
    emit interactiveTimout();
  }

  ScreenAwake();
  setAwake(s.scene.ignition || interactive_timeout > 0);
}

UIState *uiState() {
  static UIState ui_state;
  return &ui_state;
}


//  atom
void Device::ScreenAwake() 
{
  UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
  const bool draw_alerts = scene.started;
  const float speed = scene.car_state.getVEgo();



  if( scene.scr.nTime > 0 )
  {
    interactive_timeout = 30 * UI_FREQ;
    scene.scr.nTime--;
  }
  else if( scene.scr.brightness_off )
  {
    interactive_timeout = 30 * UI_FREQ;
  }
  else if( scene.ignition && (speed < 1))
  {
    interactive_timeout = 30 * UI_FREQ;
  }
  else if( scene.scr.autoScreenOff && scene.scr.nTime == 0)
  {
   // awake = false;
  }

  int  cur_key = scene.scr.awake;
  if (draw_alerts && scene.controls_state.getAlertSize() != cereal::ControlsState::AlertSize::NONE) 
  {
      cur_key += 1;
  }

  static int old_key;
  if( cur_key != old_key )
  {
    old_key = cur_key;
    if(cur_key)
    {
      setAwake(true);
      resetInteractiveTimout();
    }
        
  } 
}
