
#include <QDialog>
#include <QDateTime>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QVBoxLayout>


#include "selfdrive/ui/qt/atom/widgets/osmWidget.h"



CosmWidget::CosmWidget( TuningPanel *panel ) : CGroupWidget( "OSM Enable" ) 
{
  m_pPanel = panel;
  QString str_param = "OpkrOSMEnable";
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


  FrameOSM( CreateBoxLayout(TP_USES) );
  refresh();
}  


void  CosmWidget::FrameOSM(QVBoxLayout *layout)
{
  // QVBoxLayout *pBoxLayout = CreateBoxLayout(TP_NONE);
  // layout->addWidget(new PidKp());
  // layout->addWidget(new PidKi());
  // layout->addWidget(new PidKd());
  // layout->addWidget(new PidKf());


 // 1.
  MenuControl *pMenu1 = new MenuControl( 
    "OpkrOSMCurvDecelOption",
    "Curv Decel Option",
    "0.None 1.Vision+OSM 2.Vision Only  3.OSM only"
     );

  pMenu1->SetControl( 0, 3, 1 );
 // pMenu1->SetString( "None,Vision+OSM,Vision,OSM" );
  //pMenu1->SetString( 0, "None");
  layout->addWidget( pMenu1 );


  MenuControl *pMenu2 = new MenuControl( 
    "OpkrOSMDebug",
    "Debug Option",
    "0.None 1.Debug"
     );

  pMenu2->SetControl( 0, 1, 1 );
  layout->addWidget( pMenu2 );
}



void CosmWidget::refresh( int nID )
{
  CGroupWidget::refresh( m_nMethod );
 

  QString  str;
  switch( m_nMethod )
  {
    case TP_NONE :  str = "0.미사용";    break;
    case TP_USES :  str = "1.사용";      break;
  }

  if( m_nMethod == TP_NONE )
    method_label->setStyleSheet("background-color: #393939;");
  else
    method_label->setStyleSheet("background-color: #E22C2C;");

  method_label->setText( str ); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//

CWidgetosmConfig::CWidgetosmConfig( TuningPanel *panel ) : CGroupWidget( "OSM Config" ) 
{
  m_pPanel = panel;
  QString str_param = "OpkrOSMConfigEnable";
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


  FrameOSM( CreateBoxLayout(TP_USES) );
  refresh();
}  


void  CWidgetosmConfig::FrameOSM(QVBoxLayout *layout)
{

  // 2.
  MenuControl *pMenu3 = new MenuControl( 
    "OpkrOSM_MIN_DISTANCE_FOR_NEW_QUERY",
    "Min distance for new query",
    "MIN_DISTANCE_FOR_NEW_QUERY Minimum distance to query area edge before issuing a new query.(mts) def:1000"
     );

  pMenu3->SetControl( 100, 9000, 10 );
  layout->addWidget( pMenu3 );  

  // 3.
  MenuControl *pMenu4 = new MenuControl( 
    "OpkrOSM_FULL_STOP_MAX_SPEED",
    "Full stop max speed",
    "FULL_STOP_MAX_SPEED Max speed for considering car is stopped.(m/s) def:1.39"
     );

  pMenu4->SetControl( 0, 100, 0.1 );
  layout->addWidget( pMenu4 );

  // 4.
  MenuControl *pMenu5 = new MenuControl( 
    "OpkrOSM_LOOK_AHEAD_HORIZON_TIME",
    "Look ahead horizon time",
    "LOOK_AHEAD_HORIZON_TIME Time horizon for look ahead of turn speed sections to provide on liveMapData msg.(s) def:15"
     );

  pMenu5->SetControl( 0, 100, 0.1 );
  layout->addWidget( pMenu5 );  

  // 7.
  MenuControl *pMenu7 = new MenuControl( 
    "OpkrOSM_PRE_LANE_DISTANCE",
    "Pre Lane Distance",
    "PRE_LANE_DISTANCE. Distance to pre-detect the position of the curve shown on the map.  def:100"
     );

  pMenu7->SetControl( 0, 500, 10 );
  layout->addWidget( pMenu7 ); 
  
}



void CWidgetosmConfig::refresh( int nID )
{
  CGroupWidget::refresh( m_nMethod );
 

  QString  str;
  switch( m_nMethod )
  {
    case TP_NONE :  str = "0.미사용";    break;
    case TP_USES :  str = "1.사용";      break;
  }

  if( m_nMethod == TP_NONE )
    method_label->setStyleSheet("background-color: #393939;");
  else
    method_label->setStyleSheet("background-color: #E22C2C;");

  method_label->setText( str ); 
}



////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//

CWidgetosmNodesData::CWidgetosmNodesData( TuningPanel *panel ) : CGroupWidget( "OSM NodesData" ) 
{
  m_pPanel = panel;
  QString str_param = "OpkrOSMNodesDataEnable";
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


  FrameOSM( CreateBoxLayout(TP_USES) );
  refresh();
}  


void  CWidgetosmNodesData::FrameOSM(QVBoxLayout *layout)
{

   // NodesData
   // 1.
  MenuControl *pMenu1 = new MenuControl( 
    "OpkrOSM_TURN_CURVATURE_THRESHOLD",
    "Turn curvature threshold",
    "_TURN_CURVATURE_THRESHOLD A curvature over this value will generate a speed limit section. 1/mts. def:0.002"
     );

  pMenu1->SetControl( 0, 1, 0.001 );
  layout->addWidget( pMenu1 );  

   // 9.
  MenuControl *pMenu9 = new MenuControl( 
    "OpkrOSM_DIVERTION_SEARCH_RANGE1",
    "Divertion search range1",
    "_DIVERTION_SEARCH_RANGE1 Range of distance to current location for divertion search.  mt. def:-200"
     );

  pMenu9->SetControl( -500, 0, 1 );
  layout->addWidget( pMenu9 );
  
  MenuControl *pMenu10 = new MenuControl( 
    "OpkrOSM_DIVERTION_SEARCH_RANGE2",
    "Divertion search range2",
    "_DIVERTION_SEARCH_RANGE2 Range of distance to current location for divertion search. mt.  def:50"
     );

  pMenu10->SetControl( 40, 60, 1 );
  layout->addWidget( pMenu10 );                     

}



void CWidgetosmNodesData::refresh( int nID )
{
  CGroupWidget::refresh( m_nMethod );
 

  QString  str;
  switch( m_nMethod )
  {
    case TP_NONE :  str = "0.미사용";    break;
    case TP_USES :  str = "1.사용";      break;
  }

  if( m_nMethod == TP_NONE )
    method_label->setStyleSheet("background-color: #393939;");
  else
    method_label->setStyleSheet("background-color: #E22C2C;");

  method_label->setText( str ); 
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//



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
