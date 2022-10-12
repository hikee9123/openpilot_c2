
#include <QDialog>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "TunWidget.h"

#include "opkr.h"

CTunWidget::CTunWidget( TuningPanel *panel, QWidget *parent) : QFrame(parent) 
{
  m_pPanel = panel;

  m_bShow = 0;
  memset( m_pChildFrame, 0, sizeof(m_pChildFrame) );



  QString str_param = "OpkrLateralControlMethod";

  auto str = QString::fromStdString( params.get( str_param.toStdString() ) );
  int value = str.toInt();
  m_nMethod = value; 

  main_layout = new QVBoxLayout(this);
  main_layout->setMargin(0);


  hlayout = new QHBoxLayout;
  hlayout->setMargin(0);
  hlayout->setSpacing(20);

  // left icon 
  pix_plus =  QPixmap( "../assets/offroad/icon_plus.png" ).scaledToWidth(80, Qt::SmoothTransformation);
  pix_minus =  QPixmap( "../assets/offroad/icon_minus.png" ).scaledToWidth(80, Qt::SmoothTransformation);


  icon_label = new QLabel();
  icon_label->setPixmap(pix_plus );
  icon_label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
  hlayout->addWidget(icon_label);

  // title
  QString  title = "Tunning control Method";
  title_label = new QPushButton(title);
  title_label->setFixedHeight(120);
  title_label->setStyleSheet("font-size: 50px; font-weight: 400; text-align: left");
  hlayout->addWidget(title_label);

  connect(title_label, &QPushButton::clicked, [=]() {

    if( m_bShow )  m_bShow = 0;
    else   m_bShow = 1;
    refresh();
  });

  // label
  method_label = new QPushButton("0.PID"); // .setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
  method_label->setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #00A12E;
  )");
  method_label->setFixedSize( 500, 100);
  hlayout->addWidget(method_label);
  QObject::connect(method_label, &QPushButton::clicked, [=]() {
    m_nMethod += 1;
    if( m_nMethod >= LAT_ALL )
      m_nMethod = 0;

    QString values = QString::number(m_nMethod);
    //params.put("OpkrLateralControlMethod", values.toStdString());
    params.put( str_param.toStdString(), values.toStdString());        
    refresh();
  });

  QObject::connect(uiState(), &UIState::offroadTransition, [=](bool offroad) {
    //for (auto btn : findChildren<ButtonControl *>()) {
      method_label->setEnabled(offroad);

      if ( offroad )
          method_label->setStyleSheet(R"(color:white; background:green)");
      else
          method_label->setStyleSheet(R"(color:gray; background:rgb(80,80,80))");

    //}
  });

  main_layout->addLayout(hlayout);


  FramePID();
  FrameINDI();
  FrameLQR();
  FrameTOROUE();
  FrameHYBRID();


  main_layout->addStretch();
  refresh();
}

CTunWidget::~CTunWidget()
{

}

void CTunWidget::closeSettings() 
{
//  main_layout->setCurrentWidget(homeWindow);

//  if (uiState()->scene.started) {
//    homeWindow->showSidebar(false);
//  }
//    m_nCommand++;
//    MessageBuilder msg;
//    auto update_data = msg.initEvent().initUpdateEvents();

//    update_data.setVersion(1);
//    update_data.setType( m_nMethod );    
//    update_data.setCommand( m_nCommand );

 //   pm->send("updateEvents", msg);

  //  m_bShow = 0;
 //   refresh();
}



void CTunWidget::FramePID(QVBoxLayout *parent) 
{
  QVBoxLayout *box_layout = parent;
  if( parent == nullptr )
      box_layout = CreateBoxLayout(LAT_PID);

  MenuControl *pKp = new MenuControl( 
    "PidKp",
    "Kp",
    "Adjust Kp def:0.25"
    );
  pKp->SetControl( 0.1, 1, 0.1 );
  box_layout->addWidget( pKp );          

  MenuControl *pKi = new MenuControl( 
    "PidKi",
    "Ki",
    "Adjust Ki def:0.05"
    );
  pKi->SetControl( 0.0, 0.1, 0.01 );
  box_layout->addWidget( pKi );

  MenuControl *pKf = new MenuControl( 
    "PidKf",
    "Kf",
    "Adjust Kf def:0.00005"
    );
  pKf->SetControl( 0.0, 0.1, 0.00001 );
  box_layout->addWidget( pKf );

  //box_layout->addWidget(new PidKp());
  //box_layout->addWidget(new PidKi());
  //box_layout->addWidget(new PidKd());
  //box_layout->addWidget(new PidKf());
  
  /*
  MenuControl *pMenu2 = new MenuControl( 
    "OpkrMaxDriverAngleWait",
    "Driver to Steer",
    "Controls smooth torque by the driver  From OpkrMaxSteeringAngle. def:0.002(5sec)",
    "../assets/offroad/icon_shell.png"    
    );
  pMenu2->SetControl( 0, 1, 0.001 );
  pMenu2->SetString( 0, "Not");
  box_layout->addWidget( pMenu2 ); 
*/

  m_pPanel->ConfirmButton( box_layout );
}

void CTunWidget::FrameINDI(QVBoxLayout *parent) 
{
  QVBoxLayout *box_layout = parent;
  if( parent == nullptr )
    box_layout = CreateBoxLayout(LAT_INDI);

  box_layout->addWidget(new InnerLoopGain());
  box_layout->addWidget(new OuterLoopGain());
  box_layout->addWidget(new TimeConstant());
  box_layout->addWidget(new ActuatorEffectiveness());
}


void  CTunWidget::FrameLQR(int nMode,QVBoxLayout *parent)
{
  QVBoxLayout *box_layout = parent;
  if( parent == nullptr )  
     box_layout = CreateBoxLayout(LAT_LQR);

  MenuControl *pScale = new MenuControl( 
    "LqrScale",
    "Scale",
    "Adjust Scale def:2000"
    );
  pScale->SetControl( 1000, 5000, 50 );
  box_layout->addWidget( pScale );      

  MenuControl *pKi = new MenuControl( 
    "LqrKi",
    "Ki",
    "Adjust Ki def:0.01"
    );
  pKi->SetControl( 0.0, 1, 0.1 );
  box_layout->addWidget( pKi ); 

  MenuControl *pGain = new MenuControl( 
    "LqrDcGain",
    "DcGain",
    "Adjust DcGain def:0.0030"
    );
  pGain->SetControl( 0.0010, 0.0050, 0.0001 );
  box_layout->addWidget( pGain ); 

  if( nMode == 0 )
    m_pPanel->ConfirmButton( box_layout );
}


void  CTunWidget::FrameTOROUE(int nMode, QVBoxLayout *parent)
{
  QVBoxLayout *box_layout = parent;
  if( parent == nullptr )    
     box_layout = CreateBoxLayout(LAT_TOROUE);

  
  MenuControl *pMaxLat = new MenuControl( 
    "TorqueMaxLatAccel",
    "MaxLatAccel",
    "Adjust MaxLatAccel def:3"
    );
  pMaxLat->SetControl( 1, 5, 0.1 );
  box_layout->addWidget( pMaxLat ); 

   MenuControl *pTorqFriction = new MenuControl( 
    "TorqueFriction",
    "Friction",
    "Adjust Friction def:0.01"
    );
   pTorqFriction->SetControl( 0, 0.2, 0.01 ); 
   box_layout->addWidget( pTorqFriction );


   MenuControl *pDeadzone = new MenuControl( 
    "Torquedeadzone",
    "Steer deadzone",
    "Adjust steering angle deadzone deg def:0"
    );
   pDeadzone->SetControl( 0, 5, 0.1 ); 
   box_layout->addWidget( pDeadzone );

   MenuControl *pLiveTuning = new MenuControl( 
    "TorqueLiveTuning",
    "Live Tuning",
    "Tune while driving. def:1"
    );
   pLiveTuning->SetControl( 0, 1, 1 ); 
   box_layout->addWidget( pLiveTuning );

  box_layout->addWidget(horizontal_line());

  box_layout->setSpacing(1);
  MenuControl *PKp = new MenuControl( 
    "TorqueKp",
    "Kp",
    "Adjust Kp def:1.0"
    );
   PKp->SetControl( 0.1, 3, 0.1 ); 
   box_layout->addWidget( PKp );

   MenuControl *PKf = new MenuControl( 
    "TorqueKf",
    "Kf",
    "Adjust Kf  def:1.0"
    );
   PKf->SetControl( 0.1, 3, 0.1 ); 
   box_layout->addWidget( PKf );   

   MenuControl *PKi = new MenuControl( 
    "TorqueKi",
    "Ki",
    "Adjust Ki  def:0.1"
    );
   PKi->SetControl( 0.0, 1, 0.01 ); 
   box_layout->addWidget( PKi );  


  box_layout->addWidget(new TorqueUseAngle());

  if( nMode == 0 )
    m_pPanel->ConfirmButton( box_layout );
}

void  CTunWidget::FrameHYBRID(QVBoxLayout *parent)
{
  QVBoxLayout *box_layout = parent;
  if( parent == nullptr )     
      box_layout = CreateBoxLayout(LAT_HYBRID);


  box_layout->addWidget(new AbstractControl("[2.LQR]","lqr","../assets/offroad/icon_shell.png"));
  FrameLQR( 1, box_layout );

  box_layout->addWidget(new AbstractControl("[3.TORQUE]","torque","../assets/offroad/icon_shell.png"));
  FrameTOROUE( 1,  box_layout );

   MenuControl *pHybridSpeed = new MenuControl( 
    "TorqueHybridSpeed",
    "Hybrid Speed",
    "Adjust Hybrid speed def:50"
    );
   pHybridSpeed->SetControl( 10, 80, 5 ); 
   box_layout->addWidget( pHybridSpeed );  


    m_pPanel->ConfirmButton( box_layout );   
}


QVBoxLayout *CTunWidget::CreateBoxLayout( int nID )
{
  QFrame  *pFrame = new QFrame(); 
  m_pChildFrame[nID] = pFrame;
  pFrame->setContentsMargins(40, 10, 40, 50);
  pFrame->setStyleSheet(R"(
    * {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: black;
    } 
  )");
  
  main_layout->addWidget(pFrame);

  QVBoxLayout *layout = new QVBoxLayout(pFrame);

  return  layout;
}

void  CTunWidget::FrameHide( int nID )
{
  for( int i = 0; i<LAT_ALL; i++ )
  {
    if( nID >= 0 && i != nID ) continue; 
    if( m_pChildFrame[i] )
      m_pChildFrame[i]->hide();
  }
}


void  CTunWidget::FrameShow( int nID )
{
  for( int i = 0; i<LAT_ALL; i++ )
  {
    if( i != nID ) continue;
    if( m_pChildFrame[i] )
      m_pChildFrame[i]->show();
  }
}


void CTunWidget::showEvent(QShowEvent *event) 
{
  refresh();
}

void CTunWidget::hideEvent(QHideEvent *event) 
{
  m_bShow = 0;
  refresh();
}

void CTunWidget::refresh() 
{
  QString str;

  switch( m_nMethod )
  {
    case LAT_PID : str = "0.PID"; break;
    case LAT_INDI : str = "1.INDI"; break;
    case LAT_LQR : str = "2.LQR";  break;
    case LAT_TOROUE : str = "3.TORQUE";  break;
    case LAT_HYBRID : str = "4.HYBRID";  break;
    case LAT_MULTI : str = "5.MULTI";  break;
    case LAT_DEFAULT : str = "6.DEFAULT";  break;
    default: str = "DEFAULT"; break;
  }


  method_label->setText( str );


  if(  m_bShow == 0 )
  {
    FrameHide();
    icon_label->setPixmap(pix_plus);
  }
  else
  {
    FrameHide();
    FrameShow( m_nMethod );
    icon_label->setPixmap(pix_minus);
  }

}



