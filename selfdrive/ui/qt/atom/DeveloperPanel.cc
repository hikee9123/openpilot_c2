
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
// #include <QPushButton>


#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/qt/widgets/offroad_alerts.h"
#include "selfdrive/ui/qt/widgets/controls.h"

#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/ui/ui.h"

#include "DeveloperPanel.h"

// 일부 코드 OPKR 참고.



DeveloperPanel::DeveloperPanel(QWidget* parent) : QFrame(parent)
{
  main_widget = new QWidget(parent);  

  main_layout = new QVBoxLayout(main_widget);
  main_layout->setSpacing(10);

  // wifi + tethering buttons
  main_layout->addWidget(new GitHash());




  main_layout->setMargin(100);
  setLayout(main_layout);


  layout()->addWidget(horizontal_line());


  layout()->addWidget(new CLiveSteerRatioToggle());
  layout()->addWidget(new BrightnessControl());
  layout()->addWidget(new CVolumeControl());  
  layout()->addWidget(new AutoScreenOff());
  layout()->addWidget(new BrightnessOffControl());
  
  layout()->addWidget(new CAutoFocus());

  

  layout()->addWidget(horizontal_line());



  auto car_interfaces = new ButtonControl("car interfaces 실행", "실행",
                                        "/data/openpilot/selfdrive/car/tests/test_car_interfaces.py 을 실행 합니다.");
  connect(car_interfaces, &ButtonControl::clicked, [=]() 
  { 
          if (ConfirmationDialog::confirm("Are you sure you want to exec(test_car_interfaces.py)?", this)) 
          {
            std::system("python /data/openpilot/selfdrive/car/tests/test_car_interfaces.py");
          }
  });

  auto build_exe = new ButtonControl("build 실행", "실행",
                                    "/data/openpilot/selfdrive/manager/build.py 을 실행 합니다.");
  connect(build_exe, &ButtonControl::clicked, [=]() 
  { 
          if (ConfirmationDialog::confirm("Are you sure you want to exec(build.py)?", this)) 
          {
            std::system("python /data/openpilot/selfdrive/manager/build.py");
          }
  });


  auto finger_exe = new ButtonControl("핑거 2.0 실행", "실행",
                                      "/data/openpilot/selfdrive/car/fw_versions.py 을 실행 합니다.");
  connect(finger_exe, &ButtonControl::clicked, [=]() 
  { 
          if (ConfirmationDialog::confirm("Are you sure you want to exec(fw_versions.py)?", this)) 
          {
            std::system("python /data/openpilot/selfdrive/car/fw_versions.py");
          }
  });

  auto android_exe = new ButtonControl("Android Open", "Open",
                                      "Android를 Open 합니다.");
  connect(android_exe, &ButtonControl::clicked, [=]() 
  { 
          if (ConfirmationDialog::confirm("Are you sure you want to exec(Android)?", this)) 
          {
            std::system("am start -a android.settings.SETTINGS");
          }
  });


  auto apk_exe = new ButtonControl("apk.py Install", "Install",
                                        "/data/openpilot/selfdrive/assets/addon/apk/apk.py 을 실행 합니다.");
  connect(apk_exe, &ButtonControl::clicked, [=]() 
  { 
          if (ConfirmationDialog::confirm("Are you sure you want to exec(apk.py)?", this)) 
          {
            std::system("chmod 777 /data/openpilot/selfdrive/assets/addon/apk/apk.py");
            std::system("chmod 777 /data/openpilot/selfdrive/assets/addon/sh/gitcommit.sh");
            std::system("chmod 777 /data/openpilot/selfdrive/assets/addon/sh/panda_flashing.sh");
            std::system("chmod 777 /data/openpilot/selfdrive/assets/addon/sh/gitpull.sh");
            std::system("chmod 777 /data/openpilot/selfdrive/assets/addon/sh/gitpull_cancel.sh");
            std::system("chmod 777 /data/openpilot/selfdrive/assets/addon/sh/run_mixplorer.sh");
            std::system("python /data/openpilot/selfdrive/assets/addon/apk/apk.py");
          }
  });

  auto mixplorer_exe = new ButtonControl("com.mixplorer Open", "Open",
                                   "run_mixplorer.sh 을 실행 합니다.");
  connect(mixplorer_exe, &ButtonControl::clicked, [=]() 
  { 
        std::system("am start -n com.mixplorer/com.mixplorer.activities.BrowseActivity");
  });

  auto tmapopen_exe = new ButtonControl("주행로그 전부 삭제", "실행");
  connect(tmapopen_exe, &ButtonControl::clicked, [=]() 
  { 
      const char* realdata_del = "rm -rf /storage/emulated/0/realdata/*";
      if (ConfirmationDialog::confirm("저장된 주행로그를 모두 삭제합니다. 진행하시겠습니까?", this)){
        std::system(realdata_del);
      }
  });


  auto tmapclose_exe = new ButtonControl("NAVI Return", "Open","NAVI을 Return 합니다.");
  connect(tmapclose_exe, &ButtonControl::clicked, [=]() 
  { 
    Params param = Params();
    auto str = QString::fromStdString(Params().get("OpkrNaviSelect"));
    int param_navi_sel = str.toInt();    


    if( param_navi_sel == 1 )
      std::system("am start --activity-task-on-home com.mnsoft.mappyobn/com.mnsoft.mappy.MainActivity");
    else if( param_navi_sel == 2 )
      std::system("am start --activity-task-on-home com.thinkware.inaviair/com.thinkware.inaviair.UIActivity");


    UIScene  &scene =  uiState()->scene;
    scene.scr.IsViewNavi = 1;
    if ( ConfirmationDialog::alert("Navi program Running", this) )
    {
      std::system("am start --activity-task-on-home com.opkr.maphack/com.opkr.maphack.MainActivity");
    }
  });


  auto softkey_exe = new ButtonControl("Soft Key Open", "Open","Soft Key 을 실행 합니다.");
  connect(softkey_exe, &ButtonControl::clicked, [=]() 
  { 
    std::system("am start com.gmd.hidesoftkeys/com.gmd.hidesoftkeys.MainActivity");

    // com.moon.adroid.level.BubbleApplication
  });  




  for (auto btn : {car_interfaces, build_exe, finger_exe, android_exe, apk_exe, mixplorer_exe, tmapopen_exe, tmapclose_exe, softkey_exe}) {
    if (btn) {
     // layout()->addWidget(horizontal_line());
      connect(parent, SIGNAL(offroadTransition(bool)), btn, SLOT(setEnabled(bool)));
      layout()->addWidget(btn);
    }
  }

  layout()->addWidget(horizontal_line());
}

void DeveloperPanel::showEvent(QShowEvent *event) 
{

}



////////////////////////////////////////////////////////////////////////////////////////
//
//  Git

GitHash::GitHash() : AbstractControl("Check for Update") 
{
  params = Params();

  local_hash.setAlignment(Qt::AlignVCenter);
  local_hash.setStyleSheet("color: #aaaaaa");

  updateBtn = new QPushButton("CHECK");
 // updateBtn->setVisible(false);
  updateBtn->setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #4a4a4a;
    }
    QPushButton:disabled {
      color: #33E4E4E4;
    }
  )");
  updateBtn->setFixedSize(250, 100); 


  hlayout->addWidget(&local_hash);
  hlayout->addWidget(updateBtn);
  //hlayout->addWidget(update_notif, 0, Qt::AlignHCenter | Qt::AlignRight);

  if( description == nullptr )
  {
    description = new QLabel(str_desc);
  }
  else
  {
    description-> setText( str_desc );
  }

    
    //description->setContentsMargins(40, 20, 40, 20);
    //description->setStyleSheet("font-size: 40px; color: grey");
    //description->setWordWrap(true);
    //description->setVisible(false);
    //main_layout->addWidget(description);



  win_widget = new QWidget;
  QVBoxLayout *vlayout = new QVBoxLayout(win_widget);
  vlayout->setMargin(0);
  vlayout->setSpacing(20);

  auto exe_git_pull = new ButtonControl("  Git Pull", "UPDATE");
  QObject::connect(exe_git_pull, &ButtonControl::clicked, [=]() 
  { 
        if (ConfirmationDialog::confirm("GitPull을 실행후 프로그램을 Update합니다. 진행하시겠습니까?", this))
        {
          std::system( "cd /data/openpilot; rm -f prebuilt" );
          const char* gitpull = "/data/openpilot/selfdrive/assets/addon/sh/gitpull.sh";
          std::system( gitpull );
        }
  });
  vlayout->addWidget( exe_git_pull );

  auto exe_git_cancel = new ButtonControl("  Git Pull cancel", "Cancel");
  QObject::connect(exe_git_cancel, &ButtonControl::clicked, [=]() 
  { 
        if (ConfirmationDialog::confirm("GitPull 이전 상태로 되돌립니다. 진행하시겠습니까?", this))
        {
          const char* gitpull_cancel = "/data/openpilot/selfdrive/assets/addon/sh/gitpull_cancel.sh ''";
          std::system(gitpull_cancel);
        }
  });
  vlayout->addWidget( exe_git_cancel );


  vlayout->addWidget( new CPrebuiltToggle() );
  main_layout->addWidget( win_widget );
  win_widget->hide();


  QObject::connect( title_label, &QPushButton::clicked, this, &GitHash::information);
  QObject::connect( updateBtn, &QPushButton::clicked, this, &GitHash::update);



  fs_watch = new QFileSystemWatcher(this);
  QObject::connect(fs_watch, &QFileSystemWatcher::fileChanged, [=](const QString path) {
      updateBtn->setText(tr("CHECK"));
      updateBtn->setEnabled(true);

      QString lhash = QString::fromStdString(params.get("GitCommit").substr(0, 10));
      QString rhash = QString::fromStdString(params.get("GitCommitRemote").substr(0, 10));
      if (lhash != rhash) {
        win_widget->show();        
      }

      emit showDescriptionEvent();
      description->setVisible( true );
      refresh();
  });

  refresh();
}

void GitHash::update()
{
    if (params.getBool("IsOffroad")) {
      fs_watch->addPath(QString::fromStdString(params.getParamPath("LastUpdateTime")));
      fs_watch->addPath(QString::fromStdString(params.getParamPath("UpdateFailedCount")));
      updateBtn->setText(tr("CHECKING"));
      updateBtn->setEnabled(false);
    }
    std::system("pkill -1 -f selfdrive.updated");
}

void GitHash::information()
{
      if( !description->isVisible() ) 
      {
        std::string cmd1 = "git fetch";
        std::system(cmd1.c_str());

        std::string cmd2 = "git rev-parse @{u}   > /data/params/d/GitCommitRemote";
        std::system(cmd2.c_str());

        win_widget->show();
        emit showDescriptionEvent();
      } else  {
        win_widget->hide();
      }
      description->setVisible(!description->isVisible());
      refresh();
}

void GitHash::refresh()
{
  QString lhash = QString::fromStdString(params.get("GitCommit").substr(0, 10));
  QString rhash = QString::fromStdString(params.get("GitCommitRemote").substr(0, 10));

  local_hash.setText( rhash );
  if (lhash == rhash) {
    str_desc = "로컬과 리모트가 일치합니다.";
   // updateBtn->setEnabled(false);
    local_hash.setStyleSheet("color: #aaaaaa");
  } else {
    str_desc = "업데이트가 있습니다.";
    description->setStyleSheet("color: #0099ff");
    local_hash.setStyleSheet("color: #0099ff");
   // updateBtn->setEnabled(true);       
  }  

  str_desc += QString("\nLOCAL:%1 REMOTE:%2").arg(lhash, rhash );
  description->setText( str_desc );  
}

void GitHash::showEvent(QShowEvent *event) 
{

   refresh();
}

IsCalibraionGridViewToggle::IsCalibraionGridViewToggle() 
        : ToggleControl("Calibraion Grid view", 
        "장착에 필요한 Grid 화면과 기울기를 제공합니다.."
        ) 
{
  QObject::connect(this, &IsCalibraionGridViewToggle::toggleFlipped, [=](int state) {
    UIScene  &scene =  uiState()->scene;
    scene.scr.IsCalibraionGridViewToggle = state;

  });
}


IsOpenpilotViewEnabledToggle::IsOpenpilotViewEnabledToggle() 
        : ToggleControl("주행화면 미리보기", 
        "오픈파일럿 주행화면을 미리보기 합니다.", 
        "", 
        Params().getBool("IsOpenpilotViewEnabled")) 
{
  win_widget = new QWidget;
  QHBoxLayout *hlayout = new QHBoxLayout(win_widget);
  hlayout->setMargin(0);
  hlayout->setSpacing(20);

  hlayout->addWidget( new IsCalibraionGridViewToggle() );
  main_layout->addWidget( win_widget );

  QObject::connect( title_label, &QPushButton::clicked, this, &IsOpenpilotViewEnabledToggle::refresh); 
  QObject::connect(this, &IsOpenpilotViewEnabledToggle::toggleFlipped, [=](int state) {
    char value = state ? '1' : '0';

    UIScene  &scene =  uiState()->scene;
    scene.IsOpenpilotViewEnabled = state;
    Params().put("IsOpenpilotViewEnabled", &value, 1);
    refresh();
  });

  main_layout->addStretch();

  refresh();
}

void IsOpenpilotViewEnabledToggle::refresh()
{
  UIScene  &scene =  uiState()->scene;

  if ( scene.pandaType != cereal::PandaState::PandaType::UNKNOWN) 
  {
      toggle.setEnabled(false);
      win_widget->show();
  }
  else
  {
      toggle.setEnabled(true);
      if( scene.IsOpenpilotViewEnabled )  win_widget->show();
      else win_widget->hide();
  }
}

void IsOpenpilotViewEnabledToggle::showEvent(QShowEvent *event) 
{
// Params params = Params(); 
   refresh();
}
////////////////////////////////////////////////////////////////////////////////////////
//
//  CLiveSteerRatioToggle

CLiveSteerRatioToggle::CLiveSteerRatioToggle() : AbstractControl("Live SteerRatio 사용", "가변/고정 SR 대신 Live SteerRatio를 사용합니다. 반학습:SteerRatio값 학습.", "../assets/offroad/icon_shell.png") 
{
  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrLiveSteerRatio"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    }

    QString values = QString::number(value);
    Params().put("OpkrLiveSteerRatio", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrLiveSteerRatio"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 2 ) {
      value = 2;
    }

    QString values = QString::number(value);
    Params().put("OpkrLiveSteerRatio", values.toStdString());
    refresh();
  });
  refresh();
}

void CLiveSteerRatioToggle::refresh() 
{
  QString option = QString::fromStdString(Params().get("OpkrLiveSteerRatio"));
  if (option == "0") 
  {
    label.setText(QString::fromStdString("학습"));
  } 
  else if (option == "1") 
  {
    label.setText(QString::fromStdString("반학습"));
  }
  else if (option == "2") 
  {
    label.setText(QString::fromStdString("고정"));
  } 
  else 
  {
    label.setText( option );
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}


////////////////////////////////////////////////////////////////////////////////////////
//
//  BrightnessControl


BrightnessControl::BrightnessControl() : AbstractControl("EON 밝기 조절(%)", "EON화면의 밝기를 조절합니다.", "../assets/offroad/icon_shell.png") 
{
  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrUIBrightness"));
    int value = str.toInt();
    value = value - 5;
    if (value <= 0 ) {
      value = 0;
    } else {
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.brightness = value;
    QString values = QString::number(value);
    Params().put("OpkrUIBrightness", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrUIBrightness"));
    int value = str.toInt();
    value = value + 5;
    if (value >= 100 ) {
      value = 100;
    } else {
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.brightness = value;
    QString values = QString::number(value);
    Params().put("OpkrUIBrightness", values.toStdString());
    refresh();
  });
  refresh();
}

void BrightnessControl::refresh() 
{
  QString option = QString::fromStdString(Params().get("OpkrUIBrightness"));
  if (option == "0") {
    label.setText(QString::fromStdString("자동조절"));
  } else {
    label.setText(QString::fromStdString(Params().get("OpkrUIBrightness")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}


CVolumeControl::CVolumeControl() : AbstractControl("EON 볼륨 조절(%)", "EON의 볼륨을 조절합니다. 안드로이드 기본값/수동설정", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrUIVolumeBoost"));
    int value = str.toInt();
    value = value - 10;
    if (value < 0 ) {
      value = 0;
    } 
    QString values = QString::number(value);

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.nVolumeBoost = value;
    Params().put("OpkrUIVolumeBoost", values.toStdString());
    refresh();
   // QUIState::ui_state.sound->volume = value * 0.005;
   // QUIState::ui_state.sound->play(AudibleAlert::CHIME_WARNING1);
  });
  
  QObject::connect(&btnplus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrUIVolumeBoost"));
    int value = str.toInt();
    value = value + 10;
    if (value > 100 ) {
      value = 100;
    } 
    QString values = QString::number(value);

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.nVolumeBoost = value;
    Params().put("OpkrUIVolumeBoost", values.toStdString());
    refresh();
  //  QUIState::ui_state.sound->volume = value * 0.005;
   // QUIState::ui_state.sound->play(AudibleAlert::CHIME_WARNING1);
  });
  refresh();
}

void CVolumeControl::refresh() {
  QString option = QString::fromStdString(Params().get("OpkrUIVolumeBoost"));
  if (option == "0") {
    label.setText(QString::fromStdString("기본값"));
  } else {
    label.setText(QString::fromStdString(Params().get("OpkrUIVolumeBoost")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

////////////////////////////////////////////////////////////////////////////////////////
//
//  AutoScreenOff


AutoScreenOff::AutoScreenOff() : AbstractControl("EON 화면 끄기(분)", "주행 시작 후 화면보호를 위해 이온화면이 꺼지는 시간을 설정합니다. 터치나 이벤트 발생시 자동으로 켜집니다.", "../assets/offroad/icon_shell.png") 
{

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrAutoScreenOff"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    } else {
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.autoScreenOff = value;
    QString values = QString::number(value);
    Params().put("OpkrAutoScreenOff", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrAutoScreenOff"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 10 ) {
      value = 10;
    } else {
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.autoScreenOff = value;
    QString values = QString::number(value);
    Params().put("OpkrAutoScreenOff", values.toStdString());
    refresh();
  });
  refresh();
}

void AutoScreenOff::refresh() 
{
  QString option = QString::fromStdString(Params().get("OpkrAutoScreenOff"));
  if (option == "0") {
    label.setText(QString::fromStdString("항상켜기"));
  } else {
    label.setText(QString::fromStdString(Params().get("OpkrAutoScreenOff")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}


BrightnessOffControl::BrightnessOffControl() : AbstractControl("EON 화면끄기 밝기(%)", "EON 화면 꺼짐 기능 사용시 자동밝기 비율에 따라 밝기를 감소시킵니다.", "../assets/offroad/icon_shell.png") {

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);


  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIBrightnessOff"));
    int value = str.toInt();
    value = value - 10;
    if (value <= 0 ) {
      value = 0;
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.brightness_off = value;
    QString values = QString::number(value);
    params.put("OpkrUIBrightnessOff", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    auto str = QString::fromStdString(params.get("OpkrUIBrightnessOff"));
    int value = str.toInt();
    value = value + 10;
    if (value >= 100 ) {
      value = 100;
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.brightness_off = value;
    QString values = QString::number(value);
    params.put("OpkrUIBrightnessOff", values.toStdString());
    refresh();
  });
  refresh();
}

void BrightnessOffControl::refresh() {
  QString option = QString::fromStdString(params.get("OpkrUIBrightnessOff"));
  if (option == "0") {
    label.setText(QString::fromStdString("완전꺼짐"));
  } else {
    label.setText(QString::fromStdString(params.get("OpkrUIBrightnessOff")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}

////////////////////////////////////////////////////////////////////////////////////////
//
//  Auto Focus

  

CAutoFocus::CAutoFocus() : AbstractControl("Auto Focus", "Focus을 변경합니다.  0:auto  1~100:manaul", "../assets/offroad/icon_shell.png") 
{

  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet("color: #e0e879");
  hlayout->addWidget(&label);

  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnminus.setFixedSize(150, 100);
  btnplus.setFixedSize(150, 100);
  hlayout->addWidget(&btnminus);
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnminus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrAutoFocus"));
    int value = str.toInt();
    value = value - 1;
    if (value <= 0 ) {
      value = 0;
    } else {
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.autoFocus = value;
    QString values = QString::number(value);
    Params().put("OpkrAutoFocus", values.toStdString());
    refresh();
  });
  
  QObject::connect(&btnplus, &QPushButton::released, [=]() {
    auto str = QString::fromStdString(Params().get("OpkrAutoFocus"));
    int value = str.toInt();
    value = value + 1;
    if (value >= 100 ) {
      value = 100;
    } else {
    }

    UIScene  &scene =  uiState()->scene;//QUIState::ui_state.scene;
    scene.scr.autoFocus = value;
    QString values = QString::number(value);
    Params().put("OpkrAutoFocus", values.toStdString());
    refresh();
  });
  refresh();
}

void CAutoFocus::refresh() 
{
  QString option = QString::fromStdString(Params().get("OpkrAutoFocus"));
  if (option == "0") {
    label.setText(QString::fromStdString("자동"));
  } else {
    label.setText(QString::fromStdString(Params().get("OpkrAutoFocus")));
  }
  btnminus.setText("－");
  btnplus.setText("＋");
}
