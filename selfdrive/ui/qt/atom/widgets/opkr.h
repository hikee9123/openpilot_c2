#pragma once

#include <QPushButton>
#include <QLineEdit>
#include <QSoundEffect>


#include <QComboBox>
#include <QAbstractItemView>
#include <QMessageBox>
#include <QProcess>

#include "selfdrive/hardware/hw.h"
#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/ui.h"

class SwitchOpenpilot : public ButtonControl {
  Q_OBJECT

public:
  SwitchOpenpilot();
  void executeProgram(const QString &tcmd);

private slots:
  void printMsg();
  void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
  Params params;

  QString githubid;
  QString githubrepo;
  QString githubbranch;

  QProcess *textMsgProcess;
  QMessageBox *outbox;
  QString outdata;

  void refresh();
  void getUserID(const QString &userid);
  void getRepoID(const QString &repoid);
  void getBranchID(const QString &branchid);
};

/*
class GitHash : public AbstractControl {
  Q_OBJECT

public:
  GitHash();

private:
  QLabel local_hash;
  QLabel remote_hash;
  Params params;
};

// openpilot preview

class OpenpilotView : public AbstractControl {
  Q_OBJECT

public:
  OpenpilotView();

private:
  QPushButton btn;
  QPushButton btnc;
  Params params;
  
  void refresh();
};
*/


class CarSelectCombo : public AbstractControl 
{
  Q_OBJECT

public:
  CarSelectCombo();

private:
  QPushButton btn;
  QComboBox combobox;
  Params params;

  void refresh();
};


class BranchSelectCombo : public AbstractControl 
{
  Q_OBJECT

public:
  BranchSelectCombo();

private:
  QComboBox combobox;
  Params params;
};



class TimeZoneSelectCombo : public AbstractControl 
{
  Q_OBJECT

public:
  TimeZoneSelectCombo();

private:
  QPushButton btn;
  QComboBox combobox;
  Params params;

  void refresh();
};



// UI
class AutoShutdown : public AbstractControl {
  Q_OBJECT

public:
  AutoShutdown();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class ForceShutdown : public AbstractControl {
  Q_OBJECT

public:
  ForceShutdown();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


/*
class VolumeControl : public AbstractControl {
  Q_OBJECT

public:
  VolumeControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  QSoundEffect effect;
  
  void refresh();
  void playsound();
};

class BrightnessControl : public AbstractControl {
  Q_OBJECT

public:
  BrightnessControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class BrightnessOffControl : public AbstractControl {
  Q_OBJECT

public:
  BrightnessOffControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoScreenOff : public AbstractControl {
  Q_OBJECT

public:
  AutoScreenOff();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
*/


class ChargingMin : public AbstractControl {
  Q_OBJECT

public:
  ChargingMin();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
class ChargingMax : public AbstractControl {
  Q_OBJECT

public:
  ChargingMax();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};



class RecordCount : public AbstractControl {
  Q_OBJECT

public:
  RecordCount();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class RecordQuality : public AbstractControl {
  Q_OBJECT

public:
  RecordQuality();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class MonitoringMode : public AbstractControl {
  Q_OBJECT

public:
  MonitoringMode();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};



class MonitorEyesThreshold : public AbstractControl {
  Q_OBJECT

public:
  MonitorEyesThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class NormalEyesThreshold : public AbstractControl {
  Q_OBJECT

public:
  NormalEyesThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class BlinkThreshold : public AbstractControl {
  Q_OBJECT

public:
  BlinkThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};



// Driving
class CruisemodeSelInit : public AbstractControl {
  Q_OBJECT

public:
  CruisemodeSelInit();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class LaneChangeSpeed : public AbstractControl {
  Q_OBJECT

public:
  LaneChangeSpeed();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LaneChangeDelay : public AbstractControl {
  Q_OBJECT

public:
  LaneChangeDelay();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LeftCurvOffset : public AbstractControl {
  Q_OBJECT

public:
  LeftCurvOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
class RightCurvOffset : public AbstractControl {
  Q_OBJECT

public:
  RightCurvOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class SteerAngleCorrection : public AbstractControl {
  Q_OBJECT

public:
  SteerAngleCorrection();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


/*
class SpeedLimitOffset : public AbstractControl {
  Q_OBJECT

public:
  SpeedLimitOffset();

private:
  QPushButton btn;
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
*/


class RESChoice : public AbstractControl {
  Q_OBJECT

public:
  RESChoice();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class AutoResCondition : public AbstractControl {
  Q_OBJECT

public:
  AutoResCondition();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class AutoResLimitTime : public AbstractControl {
  Q_OBJECT

public:
  AutoResLimitTime();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoEnableSpeed : public AbstractControl {
  Q_OBJECT

public:
  AutoEnableSpeed();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class CamDecelDistAdd : public AbstractControl {
  Q_OBJECT

public:
  CamDecelDistAdd();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class MaxSteer : public AbstractControl {
  Q_OBJECT

public:
  MaxSteer();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxRTDelta : public AbstractControl {
  Q_OBJECT

public:
  MaxRTDelta();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxRateUp : public AbstractControl {
  Q_OBJECT

public:
  MaxRateUp();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class MaxRateDown : public AbstractControl {
  Q_OBJECT

public:
  MaxRateDown();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class CameraOffset : public AbstractControl {
  Q_OBJECT

public:
  CameraOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PathOffset : public AbstractControl {
  Q_OBJECT

public:
  PathOffset();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SRBaseControl : public AbstractControl {
  Q_OBJECT

public:
  SRBaseControl();

private:
  QPushButton btndigit;
  QPushButton btnminus;
  QPushButton btnplus;
  QLabel label;
  Params params;
  float digit = 0.01;
  
  void refresh();
};

class SRMaxControl : public AbstractControl {
  Q_OBJECT

public:
  SRMaxControl();

private:
  QPushButton btndigit;
  QPushButton btnminus;
  QPushButton btnplus;
  QLabel label;
  Params params;
  float digit = 0.01;
  
  void refresh();
};

class SteerActuatorDelay : public AbstractControl {
  Q_OBJECT

public:
  SteerActuatorDelay();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class SteerLimitTimer : public AbstractControl {
  Q_OBJECT

public:
  SteerLimitTimer();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TireStiffnessFactor : public AbstractControl {
  Q_OBJECT

public:
  TireStiffnessFactor();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SteerMax : public AbstractControl {
  Q_OBJECT

public:
  SteerMax();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class SteerDeltaUp : public AbstractControl {
  Q_OBJECT

public:
  SteerDeltaUp();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class SteerDeltaDown : public AbstractControl {
  Q_OBJECT

public:
  SteerDeltaDown();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};


class SteerThreshold : public AbstractControl {
  Q_OBJECT

public:
  SteerThreshold();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


// control
class LateralControl : public AbstractControl {
  Q_OBJECT

public:
  LateralControl();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  int latcontrol;


  void refresh();
};

/*
class PidKp : public AbstractControl {
  Q_OBJECT

public:
  PidKp();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKi : public AbstractControl {
  Q_OBJECT

public:
  PidKi();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKd : public AbstractControl {
  Q_OBJECT

public:
  PidKd();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class PidKf : public AbstractControl {
  Q_OBJECT

public:
  PidKf();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
*/

class OuterLoopGain : public AbstractControl {
  Q_OBJECT

public:
  OuterLoopGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class InnerLoopGain : public AbstractControl {
  Q_OBJECT

public:
  InnerLoopGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TimeConstant : public AbstractControl {
  Q_OBJECT

public:
  TimeConstant();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class ActuatorEffectiveness : public AbstractControl {
  Q_OBJECT

public:
  ActuatorEffectiveness();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

/*
class Scale : public AbstractControl {
  Q_OBJECT

public:
  Scale();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class LqrKi : public AbstractControl {
  Q_OBJECT

public:
  LqrKi();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class DcGain : public AbstractControl {
  Q_OBJECT

public:
  DcGain();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
*/
class TorqueKp : public AbstractControl {
  Q_OBJECT

public:
  TorqueKp();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TorqueKf : public AbstractControl {
  Q_OBJECT

public:
  TorqueKf();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TorqueKi : public AbstractControl {
  Q_OBJECT

public:
  TorqueKi();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class TorqueUseAngle : public ToggleControl {
  Q_OBJECT

public:
  TorqueUseAngle() : ToggleControl("UseAngle", "Use Steer Angle On/Off", "../assets/offroad/icon_shell.png", Params().getBool("TorqueUseAngle")) {
    QObject::connect(this, &TorqueUseAngle::toggleFlipped, [=](int state) {
      bool status = state ? true : false;
      Params().putBool("TorqueUseAngle", status);
    });
  }
};



class CruiseGapTR : public AbstractControl {
  Q_OBJECT

public:
  CruiseGapTR();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  QPushButton btn4;
  QLabel label1;
  QLabel label2;
  QLabel label3;
  QLabel label4;
  QLabel label1a;
  QLabel label2a;
  QLabel label3a;
  QLabel label4a;
  Params params;
  
  void refresh1();
  void refresh2();
  void refresh3();
  void refresh4();
};


class DynamicTRGap : public AbstractControl {
  Q_OBJECT

public:
  DynamicTRGap();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};



class DynamicTRUD : public AbstractControl {
  Q_OBJECT

public:
  DynamicTRUD();
};

class LCTimingFactor : public AbstractControl {
  Q_OBJECT

public:
  LCTimingFactor();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  QPushButton btn4;
  QLabel label1;
  QLabel label2;
  QLabel label3;
  QLabel label4;
  QLabel label1a;
  QLabel label2a;
  QLabel label3a;
  QLabel label4a;
  Params params;

  void refresh1();
  void refresh2();
  void refresh3();
  void refresh4();
};

class LCTimingFactorUD : public AbstractControl {
  Q_OBJECT

public:
  LCTimingFactorUD();

private:
  QPushButton btn;
  QPushButton btn2;
  Params params;
  
  void refresh();
  void refresh2();
};


class LiveSRPercent : public AbstractControl {
  Q_OBJECT

public:
  LiveSRPercent();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class VCurvSpeedUD : public AbstractControl {
  Q_OBJECT

public:
  VCurvSpeedUD();
};

class VCurvSpeed : public AbstractControl {
  Q_OBJECT

public:
  VCurvSpeed();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class OCurvSpeedUD : public AbstractControl {
  Q_OBJECT

public:
  OCurvSpeedUD();
};

class OCurvSpeed : public AbstractControl {
  Q_OBJECT

public:
  OCurvSpeed();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class GetOffAlert : public AbstractControl {
  Q_OBJECT

public:
  GetOffAlert();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};


class OPKRServerSelect : public AbstractControl {
  Q_OBJECT

public:
  OPKRServerSelect();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  Params params;
  
  void refresh();
};

class OPKRServerAPI : public AbstractControl {
  Q_OBJECT

public:
  OPKRServerAPI();

private:
  QLabel label;
  QPushButton btn;
  Params params;

  void refresh();
};

class OPKRMapboxStyle : public AbstractControl {
  Q_OBJECT

public:
  OPKRMapboxStyle();

private:
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  Params params;
  
  void refresh();
};

class RESCountatStandstill : public AbstractControl {
  Q_OBJECT

public:
  RESCountatStandstill();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SpeedLimitSignType : public AbstractControl {
  Q_OBJECT

public:
  SpeedLimitSignType();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class RadarLongHelperOption : public AbstractControl {
  Q_OBJECT

public:
  RadarLongHelperOption();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class CurvDecelSelect : public AbstractControl {
  Q_OBJECT

public:
  CurvDecelSelect();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class AutoRESDelay : public AbstractControl {
  Q_OBJECT

public:
  AutoRESDelay();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class OSMCustomSpeedLimitUD : public AbstractControl {
  Q_OBJECT

public:
  OSMCustomSpeedLimitUD();
};

class OSMCustomSpeedLimit : public AbstractControl {
  Q_OBJECT

public:
  OSMCustomSpeedLimit();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class DesiredCurvatureLimit : public AbstractControl {
  Q_OBJECT

public:
  DesiredCurvatureLimit();

private:
  QPushButton btndigit;
  QPushButton btnminus;
  QPushButton btnplus;
  QLabel label;
  Params params;
  float digit = 0.01;
  
  void refresh();
};

class DynamicTRBySpeed : public AbstractControl {
  Q_OBJECT

public:
  DynamicTRBySpeed();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

class LaneWidth : public AbstractControl {
  Q_OBJECT

public:
  LaneWidth();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class SpeedLaneWidthUD : public AbstractControl {
  Q_OBJECT

public:
  SpeedLaneWidthUD();
};

class SpeedLaneWidth : public AbstractControl {
  Q_OBJECT

public:
  SpeedLaneWidth();

private:
  QPushButton btn;
  QLineEdit edit1;
  QLineEdit edit2;
  Params params;

  void refresh();
};

/*
class OPKRTopTextView : public AbstractControl {
  Q_OBJECT

public:
  OPKRTopTextView();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};
*/
class OPKREdgeOffset : public AbstractControl {
  Q_OBJECT

public:
  OPKREdgeOffset();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class ToAvoidLKASFault : public AbstractControl {
  Q_OBJECT

public:
  ToAvoidLKASFault();

private:
  QPushButton btnplusl;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QPushButton btnminusr;
  QLabel labell1;
  QLabel labelr1;
  QLabel labell;
  QLabel labelr;
  Params params;
  
  void refreshl();
  void refreshr();
};

class RoutineDriveOption : public AbstractControl {
  Q_OBJECT

public:
  RoutineDriveOption();

private:
  QPushButton btn0;
  QPushButton btn1;
  Params params;
  
  void refresh();
};

class RPMAnimatedMaxValue : public AbstractControl {
  Q_OBJECT

public:
  RPMAnimatedMaxValue();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
};

class UserSpecificFeature : public AbstractControl {
  Q_OBJECT

public:
  UserSpecificFeature();

private:
  QPushButton btn;
  QLineEdit edit;
  Params params;

  void refresh();
};

class MultipleLatSelect : public AbstractControl {
  Q_OBJECT

public:
  MultipleLatSelect();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  int    m_nMethod;


  void refresh();
};

class MultipleLateralSpeed : public AbstractControl {
  Q_OBJECT

public:
  MultipleLateralSpeed();

private:
  QLabel label1;
  QPushButton btnplusl;
  QLabel labell;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QLabel labelr;
  QPushButton btnminusr;
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  Params params;
  
  void refresh1();
  void refresh2();
  void refresh3();
  void refreshl();
  void refreshr();
};

class MultipleLateralAngle : public AbstractControl {
  Q_OBJECT

public:
  MultipleLateralAngle();

private:
  QLabel label1;
  QPushButton btnplusl;
  QLabel labell;
  QPushButton btnminusl;
  QPushButton btnplusr;
  QLabel labelr;
  QPushButton btnminusr;
  QPushButton btn1;
  QPushButton btn2;
  QPushButton btn3;
  Params params;
  
  void refresh1();
  void refresh2();
  void refresh3();
  void refreshl();
  void refreshr();
};

class StoppingDist : public AbstractControl {
  Q_OBJECT

public:
  StoppingDist();

private:
  QPushButton btnplus;
  QPushButton btnminus;
  QLabel label;
  Params params;
  
  void refresh();
}; 

// ------





