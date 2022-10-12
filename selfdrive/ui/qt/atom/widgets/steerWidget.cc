
#include <QDialog>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "selfdrive/ui/qt/atom/widgets/steerWidget.h"



CSteerWidget::CSteerWidget(TuningPanel *panel, QWidget *parent) : QFrame(parent) 
{
  m_pPanel = panel;  
  m_bShow = 0;

  QString  str_param = "OpkrSteerMethod";


  auto str = QString::fromStdString( params.get( str_param.toStdString() ) );
  int value = str.toInt();
  m_nSelect = value; 

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
  QString  title = "Steer fail control Method";
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
  method_label = new QPushButton("method"); // .setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
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
  connect(method_label, &QPushButton::clicked, [=]() {
    m_nSelect += 1;
    if( m_nSelect > 1 )
      m_nSelect = 0;

    QString values = QString::number(m_nSelect);
    params.put( str_param.toStdString(), values.toStdString());    
    refresh();
  });

  main_layout->addLayout(hlayout);


  FrameSmooth( parent );
  FrameNormal( parent );


  main_layout->addStretch();
  refresh();
}

CSteerWidget::~CSteerWidget()
{

}


void CSteerWidget::FrameSmooth(QWidget *parent) 
{
 // 1. layer#1 menu
  m_pChildFrame1 = new QFrame(); 
  m_pChildFrame1->setContentsMargins(40, 10, 40, 50);
  m_pChildFrame1->setStyleSheet(R"(
    * {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: black;
    } 
  )");
  
  main_layout->addWidget(m_pChildFrame1);


  QVBoxLayout *menu_layout = new QVBoxLayout(m_pChildFrame1);



  MenuControl *pMenu1 = new MenuControl( 
    "OpkrMaxSteeringAngle",
    "Driver to Steer Angle",
    "mprove the edge between the driver and the openpilot."
     );
  pMenu1->SetControl( 10, 180, 5 );
  menu_layout->addWidget( pMenu1 );

  
   MenuControl *pMenu2 = new MenuControl( 
    "OpkrMaxDriverAngleWait",
    "Driver to Steer",
    "Controls smooth torque by the driver  From OpkrMaxSteeringAngle. def:0.002(5sec)"
    );
  pMenu2->SetControl( 0, 1, 0.001 );
  pMenu2->SetString( 0, "Not");
  menu_layout->addWidget( pMenu2 ); 


   MenuControl *pMenu3 = new MenuControl( 
    "OpkrMaxSteerAngleWait" ,
    "Steer angle",
    "Controls torque by steering angle From OpkrMaxSteeringAngle. def:0.001(10sec)"
    );
  pMenu3->SetControl( 0, 1, 0.001 );
  pMenu3->SetString( 0, "Not");
  menu_layout->addWidget( pMenu3 ); 

   MenuControl *pMenu4 = new MenuControl( 
    "OpkrDriverAngleWait" ,
    "Normal driver",
    "Controls torque limitation due to normal driver handle intervention. def:0.001(10sec)"
    );
  pMenu4->SetControl( 0, 1, 0.001 );
  pMenu4->SetString( 0, "Not");
  menu_layout->addWidget( pMenu4 ); 

}

void CSteerWidget::FrameNormal(QWidget *parent) 
{
 // 1. layer#2 menu
  m_pChildFrame2 = new QFrame(); 
  m_pChildFrame2->setContentsMargins(40, 10, 40, 50);
  m_pChildFrame2->setStyleSheet(R"(
    * {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: black;
    } 
  )");
  
  main_layout->addWidget(m_pChildFrame2);
  QVBoxLayout *menu_layout = new QVBoxLayout(m_pChildFrame2);  
  MenuControl *pMenu1 = new MenuControl( 
    "OpkrMaxAngleLimit",
    "Max Steering Angle",
    "Set the maximum steering angle of the handle where the openpilot is possible. Please note that some vehicles may experience errors if the angle is set above 90 degrees."
    //"../assets/offroad/icon_chevron_right.png"    
    );
  pMenu1->SetControl( 80, 360, 10 );
  pMenu1->SetString( 80, "NoLimit");
  menu_layout->addWidget( pMenu1 );
}


void CSteerWidget::showEvent(QShowEvent *event) 
{
  refresh();
}

void CSteerWidget::hideEvent(QHideEvent *event) 
{
  m_bShow = 0;
  refresh();
}

void CSteerWidget::refresh() 
{
  QString str;

  switch( m_nSelect )
  {
    case 0 : str = "0.Normal"; break;
    case 1 : str = "1.Smooth"; break;
    default: str = "2.Empty";  break;
  }


  method_label->setText( str );


  if(  m_bShow == 0 )
  {
    // pmyWidget->setVisible(false);
    m_pChildFrame1->hide();
    m_pChildFrame2->hide();
    icon_label->setPixmap(pix_plus);
  }
  else
  {
     if( m_nSelect == 0 )
     {
       m_pChildFrame2->show();
        m_pChildFrame1->hide();
     }
     else
     {
       m_pChildFrame1->show();
       m_pChildFrame2->hide();
     }

    
    icon_label->setPixmap(pix_minus);
    //pmyWidget->setVisible(true);
  }

}



CLaneWidget::CLaneWidget( TuningPanel *panel, QWidget *parent) : QFrame(parent) 
{
  m_pPanel = panel; 
  m_bShow = 0;
  m_nSelect = 0; 


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
  QString  title = "Lane control";
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
  method_label = new QPushButton("method"); // .setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
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
  connect(method_label, &QPushButton::clicked, [=]() {
    m_nSelect += 1;
    if( m_nSelect > 1 )
      m_nSelect = 0;


    refresh();
  });

  main_layout->addLayout(hlayout);


  FrameCamera( parent );
  FrameLane( parent );



  main_layout->addStretch();
  refresh();
}

CLaneWidget::~CLaneWidget()
{

}



void CLaneWidget::FrameCamera(QWidget *parent) 
{
 // 1. layer#1 menu
  m_pChildFrame1 = new QFrame(); 
  m_pChildFrame1->setContentsMargins(40, 10, 40, 50);
  m_pChildFrame1->setStyleSheet(R"(
    * {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: black;
    } 
  )");
  
  main_layout->addWidget(m_pChildFrame1);

  QVBoxLayout *menu_layout = new QVBoxLayout(m_pChildFrame1);

  MenuControl *pMenu1 = new MenuControl( 
    "OpkrCameraOffsetAdj",
    "Camera Offset value.",
    "camera offset default:0.6"
    );
  pMenu1->SetControl( -5, 5, 0.1 );
  menu_layout->addWidget( pMenu1 );
  
   MenuControl *pMenu2 = new MenuControl( 
    "OpkrPathOffsetAdj",
    "PathOffset value.",
    "path offset default:0"
    );
  pMenu2->SetControl( -5, 5, 0.1 );
  menu_layout->addWidget( pMenu2 ); 

  m_pPanel->ConfirmButton( menu_layout, 2 );

}

void CLaneWidget::FrameLane(QWidget *parent) 
{
 // 1. layer#2 menu
  m_pChildFrame2 = new QFrame(); 
  m_pChildFrame2->setContentsMargins(40, 10, 40, 50);
  m_pChildFrame2->setStyleSheet(R"(
    * {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: black;
    } 
  )");
  
  main_layout->addWidget(m_pChildFrame2);
  QVBoxLayout *menu_layout = new QVBoxLayout(m_pChildFrame2);  
  MenuControl *pMenu1 = new MenuControl( 
    "OpkrLeftLaneOffset",
    "Left lane offset",
    "Left lane offset."
    );
  pMenu1->SetControl( -5, 5, 0.1 );
  menu_layout->addWidget( pMenu1 );


  MenuControl *pMenu2 = new MenuControl( 
    "OpkrRightLaneOffset",
    "Right lane offset",
    "Right lane offset"
    );
  pMenu2->SetControl( -5, 5, 0.1 );
  menu_layout->addWidget( pMenu2 );

  m_pPanel->ConfirmButton( menu_layout, 2 );
}


void CLaneWidget::showEvent(QShowEvent *event) 
{
  refresh();
}

void CLaneWidget::hideEvent(QHideEvent *event) 
{
  m_bShow = 0;
  refresh();
}

void CLaneWidget::refresh() 
{
  QString str;

  switch( m_nSelect )
  {
    case 0 : str = "0.Lane"; break;
    case 1 : str = "1.Camera"; break;
    default: str = "2.Empty";  break;
  }


  method_label->setText( str );


  if(  m_bShow == 0 )
  {
    m_pChildFrame1->hide();
    m_pChildFrame2->hide();
    icon_label->setPixmap(pix_plus);
  }
  else
  {
     if( m_nSelect == 0 )
     {
       m_pChildFrame2->show();
        m_pChildFrame1->hide();
     }
     else
     {
       m_pChildFrame1->show();
       m_pChildFrame2->hide();
     }

    icon_label->setPixmap(pix_minus);
  }

}


/**
 * NaviSect
 * 
 */

CNaviSelect::CNaviSelect( TuningPanel *panel ) : CGroupWidget( "Navi Select" ) 
{
  m_pPanel = panel;
  QString str_param = "OpkrNaviSelect";
  auto str = QString::fromStdString( params.get( str_param.toStdString() ) );
  int value = str.toInt();
  m_nMethod = value;    

  // label
  method_label = new QPushButton("method"); // .setAlignment(Qt::AlignVCenter|Qt::AlignHCenter);
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
  
  connect(method_label, &QPushButton::clicked, [=]() {
    m_nMethod += 1;
    if( m_nMethod >= TP_ALL )
      m_nMethod = 0;

    QString values = QString::number(m_nMethod);
    params.put( str_param.toStdString(), values.toStdString());      
    refresh();
  });
  main_layout->addLayout(hlayout);


  FrameMappy( CreateBoxLayout(TP_MAPPY) );
  FrameINavi( CreateBoxLayout(TP_INAVI) );
  refresh();
}  


void  CNaviSelect::FrameMappy(QVBoxLayout *layout)
{
   // 1.
  MenuControl *pMenu1 = new MenuControl( 
    "OpkrNavi_Mappy",
    "Mappy curvature threshold",
    "_TURN_CURVATURE_THRESHOLD A curvature over this value will generate a speed limit section. 1/mts. def:0.002"
     );

  pMenu1->SetControl( 0, 1, 0.001 );
  layout->addWidget( pMenu1 );  
}


void  CNaviSelect::FrameINavi(QVBoxLayout *layout)
{
   // 1.
  MenuControl *pMenu1 = new MenuControl( 
    "OpkrNavi_INavi",
    "INavi curvature threshold",
    "_TURN_CURVATURE_THRESHOLD A curvature over this value will generate a speed limit section. 1/mts. def:0.002"
     );

  pMenu1->SetControl( 0, 1, 0.001 );
  layout->addWidget( pMenu1 );  
}


void CNaviSelect::refresh( int nID )
{
  CGroupWidget::refresh( m_nMethod );

  QString  str;
  switch( m_nMethod )
  {
    case TP_NONE  :  str = "0.None";    break;
    case TP_MAPPY :  str = "1.Mappy";    break;
    case TP_INAVI :  str = "2.iNavi";      break;
  }

  if( m_nMethod == TP_NONE )
    method_label->setStyleSheet("background-color: #101010;");
  else
    method_label->setStyleSheet("background-color: #393939;");

  method_label->setText( str ); 
}
