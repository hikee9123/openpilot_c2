
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
// #include <QPushButton>


#include "selfdrive/ui/qt/widgets/input.h"
#include "selfdrive/ui/qt/widgets/toggle.h"
#include "selfdrive/ui/qt/widgets/offroad_alerts.h"
#include "selfdrive/ui/qt/widgets/controls.h"

#include "selfdrive/ui/qt/atom/widgets/steerWidget.h"
#include "selfdrive/ui/qt/atom/widgets/TunWidget.h"
#include "selfdrive/ui/qt/atom/widgets/osmWidget.h"

#include "selfdrive/common/params.h"
#include "selfdrive/common/util.h"
#include "selfdrive/ui/ui.h"


#include "widgets/opkr.h"

#include "TuningPanel.h"

// 일부 코드 OPKR 참고.

TuningPanel::TuningPanel(QWidget *parent) : QFrame(parent) 
{
  m_nCommand = 0;
  pm = new PubMaster({"updateEvents"});


  QVBoxLayout *main_layout = new QVBoxLayout(this);


  Params param = Params();
  int osm_enable = param.getBool("OpkrOSMEnable");



  // OPKR
  main_layout->addWidget(new CTunWidget(this));
  main_layout->addWidget(new CSteerWidget(this));  
  main_layout->addWidget(new CLaneWidget(this));  
  
  main_layout->addWidget(new CosmWidget(this)); 

  if( osm_enable )
  {
    main_layout->addWidget(new CWidgetosmConfig(this));
    main_layout->addWidget(new CWidgetosmNodesData(this));
  }

  main_layout->addWidget(new CNaviSelect(this));
}


TuningPanel::~TuningPanel()
{
}


void TuningPanel::ConfirmButton( QVBoxLayout *parent, int version ) 
{
  QPushButton* confirm_btn = new QPushButton("confirm");
  confirm_btn->setFixedSize(386, 125);
  confirm_btn->setStyleSheet(R"(
    font-size: 48px;
    border-radius: 10px;
    color: #E4E4E4;
    background-color: #444444;
  )");

  

  parent->addWidget(confirm_btn, 0, Qt::AlignRight );

  QObject::connect(confirm_btn, &QPushButton::clicked, [=]() 
  {
      m_nCommand++;
      if( m_nCommand > 99 ) m_nCommand = 0;
      
      MessageBuilder msg;
      auto update_events = msg.initEvent().initUpdateEvents();
      update_events.setVersion(version);
      update_events.setType( 0 ); // m_nMethod );    
      update_events.setCommand( m_nCommand );

      pm->send("updateEvents", msg);

      QString  strBtn;
      strBtn.sprintf("confirm(%d)", m_nCommand);
      confirm_btn->setText( strBtn );

     // m_bShow = 0;
     // refresh();
  });

 // QObject::connect(confirm_btn, &QPushButton::clicked, this, &CTunWidget::closeSettings);
}


