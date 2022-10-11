#include "selfdrive/ui/qt/atom/paint.h"

#include <cmath>

#include <QDebug>

#include "selfdrive/common/timing.h"
#include "selfdrive/ui/qt/util.h"

#include "selfdrive/logcatd/traffic_sign.h"


int OnPaint::get_param( const std::string &key )
{
    auto str = QString::fromStdString(Params().get( key ));
    int value = str.toInt();

    return value;
}

// OnroadHud
OnPaint::OnPaint(QWidget *parent) : QWidget(parent) 
{
  m_param.bbh_left = 0;
  m_param.bbh_right = 0;

  state = uiState();
  scene = &(state->scene);

  printf("Check => 1.QPixmap \n");
  img_compass= QPixmap("../assets/addon/Image/img_compass.png").scaled(img_size_compass, img_size_compass, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_direction= QPixmap("../assets/addon/Image/img_direction.png").scaled(img_size_compass, img_size_compass, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_tire_pressure= QPixmap("../assets/addon/Image/img_tire_pressure.png").scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);


  img_traf_turn= QPixmap("../assets/img_trafficSign_turn.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_speed = QPixmap("../assets/addon/navigation/img_speed.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_section = QPixmap("../assets/addon/navigation/img_section.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_camera = QPixmap("../assets/addon/navigation/img_camera.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  //img_overtrack = QPixmap("../assets/addon/navigation/overtrak.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_park_crackdown = QPixmap("../assets/addon/navigation/park_crackdown.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);


  img_speed_var= QPixmap("../assets/addon/navigation/img_var_speedahead.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_img_space= QPixmap("../assets/addon/navigation/img_space.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_car_left= QPixmap("../assets/addon/navigation/img_car_left.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_car_right= QPixmap("../assets/addon/navigation/img_car_right.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_speed_bump= QPixmap("../assets/addon/navigation/img_speed_bump.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_bus_only= QPixmap("../assets/addon/navigation/img_bus_only.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_school_zone= QPixmap("../assets/addon/navigation/img_S30_speedahead.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  img_curve_right= QPixmap("../assets/addon/navigation/img_curve_right.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_curve_left= QPixmap("../assets/addon/navigation/img_curve_left.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_narrow_road= QPixmap("../assets/addon/navigation/img_narrow_road.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_rail_road= QPixmap("../assets/addon/navigation/img_rail_road.png").scaled(img_size, img_size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  printf("Check => 2.QPixmap \n");

  connect(this, &OnPaint::valueChanged, [=] { update(); });


  scene->scr.autoFocus = get_param("OpkrAutoFocus");
  scene->scr.brightness_off = get_param("OpkrUIBrightnessOff");
  scene->scr.autoScreenOff = get_param("OpkrAutoScreenOff");
  scene->scr.brightness = get_param("OpkrUIBrightness");
  scene->scr.nTime = scene->scr.autoScreenOff * 60 * UI_FREQ;
}

void OnPaint::updateState(const UIState &s)
{

  auto deviceState = s.scene.deviceState;// sm["deviceState"].getDeviceState();

  m_param.batteryTemp = deviceState.getBatteryTempCDEPRECATED();
  m_param.cpuPerc = deviceState.getCpuUsagePercent()[0];

  if( m_param.cpuPerc < 0) m_param.cpuPerc = 0;
  else if( m_param.cpuPerc > 100) m_param.cpuPerc = 100;

  auto  maxCpuTemp = deviceState.getCpuTempC();
    m_param.cpuTemp = maxCpuTemp[0];      




  SubMaster &sm = *(s.sm);
  if (sm.frame % (UI_FREQ / 2) != 0) return;

    auto gps_ext = s.scene.gpsLocationExternal;
    m_param.gpsAccuracyUblox = gps_ext.getAccuracy();
    m_param.altitudeUblox = gps_ext.getAltitude(); 
    m_param.bearingUblox = gps_ext.getBearingDeg();




    m_param.angleSteers = s.scene.car_state.getSteeringAngleDeg();
    m_param.angleSteersDes = s.scene.controls_state.getSteeringAngleDesiredDegDEPRECATED();

    m_param.car_state = s.scene.car_state;
    auto radar_state = sm["radarState"].getRadarState();  // radar
    m_param.lead_radar = radar_state.getLeadOne();

    //if( memcmp( &m_param, &m_old, sizeof(m_param)) )
    //{
    //   m_old = m_param;
       update(); 
    //}

   // setProperty("invalidate", invalidate );
}


float OnPaint::interp( float xv, float xp[], float fp[], int N)
{
	float dResult = 0; 
	int low, hi = 0;

	while ( (hi < N) && (xv > xp[hi]))
	{
		hi += 1;
	}
	low = hi - 1;
	if( low < 0 )
	{
		low = N-1;
		return fp[0];
	}

	if (hi == N && xv > xp[low])
	{
		return fp[N-1];
	}
	else
	{
		if( hi == 0 )
		{
			return fp[0];
		}
		else
		{
			dResult = (xv - xp[low]) * (fp[hi] - fp[low]) / (xp[hi] - xp[low]) + fp[low];
		}
	}
	return  dResult;
}

void OnPaint::paintEvent(QPaintEvent *event) 
{
  QPainter p(this);
  //p.setRenderHint(QPainter::Antialiasing);

//  printf( "OnPaint::paintEvent" );

  bb_ui_draw_UI( p );
  ui_main_navi( p );
}
// 
void OnPaint::drawText(QPainter &p, int x, int y, const QString &text, QColor qColor, int nAlign ) 
{
  QFontMetrics fm(p.font());
  QRect init_rect = fm.boundingRect(text);
  QRect real_rect = fm.boundingRect(init_rect, 0, text);

  if( nAlign == Qt::AlignCenter ) // Qt::AlignLeft )
  {
     real_rect.moveCenter({x, y - real_rect.height() / 2});
  }
  else  if( nAlign ==  Qt::AlignRight  )
  {
    real_rect.moveLeft( x );
  }
  else  if( nAlign ==  Qt::AlignLeft  )
  {
    real_rect.moveRight( x );
  }
  else
  {
    real_rect.moveTo(x, y - real_rect.height() / 2);
  }


  p.setPen( qColor ); //QColor(0xff, 0xff, 0xff, alpha));
  //p.drawText(real_rect.x(), real_rect.bottom(), text);
  p.drawText(real_rect, nAlign, text);
}



//BB START: functions added for the display of various items
int OnPaint::bb_ui_draw_measure(QPainter &p,  const QString &bb_value, const QString &bb_uom, const QString &bb_label,
    int bb_x, int bb_y, int bb_uom_dx,
    QColor bb_valueColor, QColor bb_labelColor, QColor bb_uomColor,
    int bb_valueFontSize, int bb_labelFontSize, int bb_uomFontSize )
{
 


  //nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_BASELINE);
  int dx = 0;
  int nLen = bb_uom.length();
  if (nLen > 0) {
    dx = (int)(bb_uomFontSize*2.5/2);
   }


  //print value
  configFont( p, "Open Sans",  bb_valueFontSize*2, "SemiBold");
  drawText( p, bb_x-dx/2, bb_y+ (int)(bb_valueFontSize*2.5)+5, bb_value, bb_valueColor );
  //print label
  configFont( p, "Open Sans",  bb_valueFontSize*1, "Regular");
  drawText( p, bb_x, bb_y + (int)(bb_valueFontSize*2.5)+5 + (int)(bb_labelFontSize*2.5)+5, bb_label, bb_labelColor);

  //print uom
  if (nLen > 0) {

    int rx =bb_x + bb_uom_dx + bb_valueFontSize -3;
    int ry = bb_y + (int)(bb_valueFontSize*2.5/2)+25;
    configFont( p, "Open Sans",  bb_uomFontSize*2, "Regular");

    p.save();
    p.translate( rx, ry);
    p.rotate( -90 );
    p.setPen( bb_uomColor ); //QColor(0xff, 0xff, 0xff, alpha));
    p.drawText( 0, 0, bb_uom);
    //drawText( p, 0, 0, bb_uom, bb_uomColor);
    p.restore();
  }
  return (int)((bb_valueFontSize + bb_labelFontSize)*2.5) + 5;
}

QColor OnPaint::get_color( int nVal, int nRed, int nYellow ) 
{
  QColor  lab_color =  QColor(255, 255, 255, 255);

      if(nVal > nRed) {
        lab_color = QColor(255, 0, 0, 200);
      } else if( nVal > nYellow) {
        lab_color = QColor(255, 188, 3, 200);
      }

  return lab_color;
}


void OnPaint::bb_ui_draw_measures_right( QPainter &p, int bb_x, int bb_y, int bb_w ) 
{
  int bb_rx = bb_x + (int)(bb_w/2);
  int bb_ry = bb_y;
  int bb_h = 5;
  QColor lab_color = QColor(255, 255, 255, 200);
  QColor uom_color = QColor(255, 255, 255, 200);
  int value_fontSize=25;
  int label_fontSize=15;
  int uom_fontSize = 15;
  int bb_uom_dx =  (int)(bb_w /2 - uom_fontSize*2.5) ;

  if ( m_param.bbh_right > 5 )
  {
    QRect rc( bb_x, bb_y, bb_w, m_param.bbh_right);
    p.setPen(QPen(QColor(0xff, 0xff, 0xff, 100), 3)); 
    p.setBrush(QColor(0, 0, 0, 100));
    p.drawRoundedRect(rc, 20, 20); 
    p.setPen(Qt::NoPen);
  }

  QString val_str;
  QString uom_str;
  //add CPU temperature
  if( true ) 
  {

    QColor val_color = QColor(255, 255, 255, 200);

     val_color = get_color(  (int)m_param.cpuTemp, 92, 80 );
     lab_color = get_color(  (int)m_param.cpuPerc, 90, 60 );


       // temp is alway in C * 10
      val_str.sprintf("%.1f", m_param.cpuTemp );
      uom_str.sprintf("%d", m_param.cpuPerc);
      bb_h += bb_ui_draw_measure(p,  val_str, uom_str, "CPU TEMP",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );

    bb_ry = bb_y + bb_h;
  }

  //add GPU temperature
  if( 0 ) 
  {
    QColor val_color = QColor(255, 255, 255, 200);
    auto  maxGpuTemp = scene->deviceState.getGpuTempC();
    float  memTemp = scene->deviceState.getMemoryTempC();
    float gpuTemp = maxGpuTemp[0];
    
    val_color = get_color(  (int)gpuTemp, 92, 80 );
    lab_color = get_color(  (int)memTemp, 92, 80 );

       // temp is alway in C * 10
      val_str.sprintf("%.1f", gpuTemp );
      uom_str.sprintf("%.1f", memTemp);       
      bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "GPU TEMP",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );

    bb_ry = bb_y + bb_h;
  } 

   //add battery temperature
   lab_color = QColor(255, 255, 255, 200);
  if( true )
  {
    QColor val_color = QColor(255, 255, 255, 200);

    val_color = get_color( (int) m_param.batteryTemp, 50, 40 );

    uint16_t fan_speed = scene->deviceState.getFanSpeedPercentDesired();

    // temp is alway in C * 1000
    val_str.sprintf("%.1f", m_param.batteryTemp );
    uom_str.sprintf("%.1f", fan_speed * 0.001);        
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "BAT TEMP",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }



  //add grey panda GPS accuracy
  if (true) {

    QColor val_color = QColor(255, 255, 255, 200);
    //show red/orange if gps accuracy is low
     val_color = get_color( (int)m_param.gpsAccuracyUblox, 5, 2 );

    // gps accuracy is always in meters
    if(m_param.gpsAccuracyUblox > 99 || m_param.gpsAccuracyUblox == 0) {
       val_str = "None";
    }else if(m_param.gpsAccuracyUblox > 9.99) {
      val_str.sprintf("%.1f", m_param.gpsAccuracyUblox );
    }
    else {
      val_str.sprintf("%.2f", m_param.gpsAccuracyUblox );
    }
    uom_str.sprintf("%.1f", m_param.altitudeUblox); 
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "GPS PREC",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //finally draw the frame
  bb_h += 20;
  m_param.bbh_right = bb_h;
}

QColor OnPaint::angleSteersColor( int angleSteers ) 
{
    QColor val_color = QColor(255, 255, 255, 200);

    if( (angleSteers < -30) || (angleSteers > 30) ) {
      val_color = QColor(255, 175, 3, 200);
    }
    if( (angleSteers < -55) || (angleSteers > 55) ) {
      val_color = QColor(255, 0, 0, 200);
    }

    return val_color;
}

void OnPaint::bb_ui_draw_measures_left(QPainter &p, int bb_x, int bb_y, int bb_w ) 
{
  int bb_rx = bb_x + (int)(bb_w/2);
  int bb_ry = bb_y;
  int bb_h = 5;
  QColor lab_color = QColor(255, 255, 255, 200);
  QColor uom_color = QColor(255, 255, 255, 200);
  int value_fontSize=25;
  int label_fontSize=15;
  int uom_fontSize = 15;
  int bb_uom_dx =  (int)(bb_w /2 - uom_fontSize*2.5) ;





  if( m_param.bbh_left > 5 )
  {
    QRect rc( bb_x, bb_y, bb_w, m_param.bbh_left);
    p.setPen(QPen(QColor(0xff, 0xff, 0xff, 100), 3));
    p.setBrush(QColor(0, 0, 0, 100));
    p.drawRoundedRect(rc, 20, 20);
    p.setPen(Qt::NoPen);  
  }



  QString val_str;
  QString uom_str;

  //add visual radar relative distance
  if( true )
  {
    QColor val_color = QColor(255, 255, 255, 200);

    if ( m_param.lead_radar.getStatus() ) {
      //show RED if less than 5 meters
      //show orange if less than 15 meters
      float d_rel2 = m_param.lead_radar.getDRel();
      
      if((int)(d_rel2) < 15) {
        val_color = QColor(255, 188, 3, 200);
      }
      if((int)(d_rel2) < 5) {
        val_color = QColor(255, 0, 0, 200);
      }
      // lead car relative distance is always in meters
      val_str.sprintf("%d", (int)d_rel2 );
    } else {
       val_str = "-";
    }

    auto lead_cam = (*state->sm)["modelV2"].getModelV2().getLeadsV3()[0];  // camera
    if (lead_cam.getProb() > 0.1) {
      float d_rel1 = lead_cam.getX()[0];

      //uom_color = QColor(255, 255, 255, 200);
      uom_str.sprintf("%d", (int)d_rel1 );
    }
    else
    {
      uom_str = "m";
    }

    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "REL DIST",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //add visual radar relative speed
  if( true )
  {
    QColor val_color = QColor(255, 255, 255, 200);
    if ( m_param.lead_radar.getStatus() ) {
      float v_rel = m_param.lead_radar.getVRel();  
      //show Orange if negative speed (approaching)
      //show Orange if negative speed faster than 5mph (approaching fast)
      if((int)(v_rel) < 0) {
        val_color = QColor(255, 188, 3, 200);
      }
      if((int)(v_rel) < -5) {
        val_color = QColor(255, 0, 0, 200);
      }
      // lead car relative speed is always in meters
      if (scene->is_metric) {
        val_str.sprintf("%d", (int)(v_rel * 3.6 + 0.5) );
      } else {
        val_str.sprintf("%d", (int)(v_rel * 2.2374144 + 0.5));
      }
    } else {
       val_str = "-";
    }
    if (scene->is_metric) {
      uom_str = "km/h";
    } else {
      uom_str = "mph";
    }
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "REL SPEED",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //add  steering angle
  if( true )
  {
    //float angleSteers = scene->car_state.getSteeringAngleDeg();

    QColor val_color = QColor(0, 255, 0, 200);
      //show Orange if more than 30 degrees
      //show red if  more than 50 degrees

      val_color = angleSteersColor( (int)(m_param.angleSteers) );

      // steering is in degrees
      val_str.sprintf("%.1f",m_param.angleSteers);

      // steering is in degrees des
      uom_color = angleSteersColor( (int)(m_param.angleSteersDes) );
      uom_str.sprintf("%.1f",m_param.angleSteersDes);

    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "REAL STEER",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //add  desired steering angle
  if( false )
  {
   // float angleSteersDes = scene->controls_state.getSteeringAngleDesiredDegDEPRECATED();  
    uom_color = QColor(255, 255, 255, 200);
    QColor val_color = QColor(255, 255, 255, 200);


    //text3.sprintf("BF:%.1f   RL:%.1f°", , scene->scr.accel_prob[1] );

    val_str.sprintf("%.1f", scene->scr.accel_prob[0]);  // BF
    uom_str.sprintf("%.1f", scene->scr.accel_prob[1]);  // RL
    //uom_str = "";
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "GRADIENT",
      bb_rx, bb_ry, bb_uom_dx,
      val_color, lab_color, uom_color,
      value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  
  if( true )
  {
    float fEngineRpm = scene->car_state.getEngineRpm();
    int   electGearStep  = scene->car_state.getElectGearStep();
  
    uom_color = QColor(255, 255, 255, 200);
    QColor val_color = QColor(255, 255, 255, 200);

    if (  fEngineRpm <= 0 )
    {
      val_str.sprintf("EV"); 
      val_color = QColor(0, 255, 0, 200);
    }
    else 
    {
      val_str.sprintf("%.0f", fEngineRpm); 
      if( fEngineRpm > 2000 ) val_color = QColor(255, 188, 3, 200);
      else if( fEngineRpm > 3000 ) val_color = QColor(255, 0, 0, 200);
    }
    
    uom_str.sprintf("%d", electGearStep );

    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "ENGINE",
      bb_rx, bb_ry, bb_uom_dx,
      val_color, lab_color, uom_color,
      value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  //finally draw the frame
  bb_h += 20;
  m_param.bbh_left = bb_h;


}


// tpms by neokii
QColor OnPaint::get_tpms_color(float tpms) 
{
    if(tpms < 5 || tpms > 60) // N/A
        return QColor(255, 255, 255, 200);
    if(tpms < 30)
        return QColor(255, 90, 90, 200);
    return QColor(255, 255, 255, 200);
}

QString OnPaint::get_tpms_text(float tpms) 
{
    if(tpms < 5 || tpms > 200)
        return "-";

    QString str;
    str.sprintf("%.0f", round(tpms) );
    return str;
}

void OnPaint::bb_draw_tpms(QPainter &p, int viz_tpms_x, int viz_tpms_y )
{
    //const UIScene *scene = &s->scene;
    //auto car_state = (*state->sm)["carState"].getCarState();
    auto tpms = m_param.car_state.getTpms();

    const float fl = tpms.getFl();
    const float fr = tpms.getFr();
    const float rl = tpms.getRl();
    const float rr = tpms.getRr();

    const int w = 58;
    const int h = 126;
    int x = viz_tpms_x;// bdr_s + 80;
    int y = viz_tpms_y - h;// s->fb_h - bdr_s - h - 60;

    const int margin = 30;

   // drawIcon(p, x, y, img_tire_pressure, QColor(0, 0, 0, 70), 1.0);
    p.drawPixmap(x+8 , y , img_tire_pressure);

    configFont( p, "Open Sans",  55, "SemiBold");
    drawText( p, x-margin, y+45, get_tpms_text(fl)  );
    drawText( p, x+w+margin, y+45, get_tpms_text(fr)  );

    drawText( p, x-margin, y+h-15, get_tpms_text(rl)  );
    drawText( p, x+w+margin, y+h-15, get_tpms_text(rr)  );
}


//draw compass by opkr and re-designed by hoya
void OnPaint::bb_draw_compass(QPainter &p, int compass_x, int compass_y )
{
 // auto   gps_ext = scene->gpsLocationExternal;
 // float  bearingUblox = gps_ext.getBearingDeg();

  int   size =  img_size_compass * 0.5;

    p.save();
    p.setOpacity(0.8);
    p.translate( compass_x+size, compass_y+size);
    p.rotate( -m_param.bearingUblox );
    p.drawPixmap( -size , -size, img_direction );
    p.restore();

  
  p.drawPixmap(compass_x , compass_y, img_compass );
}


void OnPaint::bb_ui_draw_UI(QPainter &p)
{
  const int bb_dml_w = 180;
  const int bb_dml_x = (0 + bdr_s);
  const int bb_dml_y = (0 + bdr_s) + 220;

  const int bb_dmr_w = 180;
  const int bb_dmr_x = 0 + state->fb_w - bb_dmr_w - bdr_s;
  const int bb_dmr_y = (0 + bdr_s) + 220;

  // 1. kegman ui
  bb_ui_draw_measures_left(p, bb_dml_x, bb_dml_y, bb_dml_w);
  bb_ui_draw_measures_right(p, bb_dmr_x, bb_dmr_y, bb_dmr_w);

  // 2. tpms
  if( true )
  {
    int viz_tpms_x = state->fb_w - bb_dmr_w / 2 - 60;
    int viz_tpms_y = state->fb_h - bdr_s - 220;  
    bb_draw_tpms( p, viz_tpms_x, viz_tpms_y);
  }

  // 3. compass
  if( true )
  {
    const int compass_x = state->fb_w / 2 - 100;
    const int compass_y = state->fb_h - 150;
    bb_draw_compass( p, compass_x, compass_y );
  }
}
//BB END: functions added for the display of various items

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Nevi
//

/*
this is navigation code by OPKR, and thank you to the OPKR developer.
I love OPKR code.
*/

void OnPaint::ui_draw_traffic_sign( QPainter &p, float map_sign, float speedLimit,  float speedLimitAheadDistance ) 
{
    int  nTrafficSign = int( map_sign );

    QPixmap  *traffic_sign = NULL;

    if( nTrafficSign == TS_BEND_ROAD ) traffic_sign = &img_traf_turn;  // 굽은도로
    else if( nTrafficSign == TS_VARIABLE ) traffic_sign = &img_speed_var;  // 가변 단속. ( by opkr)
    else if( nTrafficSign == TS_BUS_ONLY ) traffic_sign = &img_bus_only;  // 버스전용차로단속
    else if( nTrafficSign == TS_BUMP_ROAD ) traffic_sign = &img_speed_bump;  // 과속방지턱
    else if( nTrafficSign == TS_SCHOOL_ZONE1 ) traffic_sign = &img_school_zone;  // 스클존
    else if( nTrafficSign == TS_SCHOOL_ZONE2 ) traffic_sign = &img_school_zone;  // 스클존
    else if( nTrafficSign == TS_CURVE_RIGHT ) traffic_sign = &img_curve_right;  // 오른쪽 급커브
    else if( nTrafficSign == TS_CURVE_LEFT ) traffic_sign = &img_curve_left;  // 왼쪽 급커브
    else if( nTrafficSign == TS_NARROW_ROAD ) traffic_sign = &img_narrow_road; // 좁아지는 도로
    else if( nTrafficSign == TS_RAIL_ROAD ) traffic_sign = &img_rail_road;   // 철길건널목
    else if( nTrafficSign == TS_PARK_CRACKDOWN ) traffic_sign = &img_park_crackdown;  // 주정차단속
    else if( nTrafficSign == TS_LANE_CHANGE1 ) traffic_sign = &img_img_space;  // 차선변경금지시작
    else if( nTrafficSign == TS_ANE_CHANGE2 ) traffic_sign = &img_img_space;  // 차선변경금지종료
    else if( nTrafficSign == TS_LOAD_OVER ) traffic_sign = &img_img_space;  // 과적단속
    else if( nTrafficSign == TS_TRAFFIC_INFO ) traffic_sign = &img_img_space;  // 교통정보수집
   // else if( nTrafficSign == TS_OVERTRAK ) traffic_sign = &img_overtrack;  // 추월금지구간
    else if( nTrafficSign == TS_SHOULDER  ) traffic_sign = &img_img_space; // 갓길단속
    else if( nTrafficSign == TS_LOAD_POOR  ) traffic_sign = &img_img_space;  // 적재불량단속  
    
    //else if( nTrafficSign == TS_CAMERA1 ) traffic_sign = &img_img_space;// 단속카메라(신호위반카메라)  
    else if( nTrafficSign == TS_CAMERA2_BUS ) traffic_sign = &img_bus_only; // 고정식  - 호야
    //else if( nTrafficSign == TS_CAMERA3 ) traffic_sign = &img_img_space; // 경찰차(이동식)  - 호야
    //else if( nTrafficSign == TS_CAMERA4 ) traffic_sign = &img_img_space; // 단속구간(고정형 이동식)
    //else if( nTrafficSign == TS_CAMERA5  ) traffic_sign = &img_img_space;  // 단속(카메라, 신호위반)    
    else if( speedLimit ) 
    {
      if( nTrafficSign == TS_INTERVAL  )   // 구간 단속
        traffic_sign = &img_section;
      else if( nTrafficSign == TS_CAMERA1  )
        traffic_sign = &img_camera;
      else
        traffic_sign = &img_speed;
    }


  



    int img_size1 = img_size;   // 472
    int img_xpos = 0 + bdr_s + 184 + 20;
    int img_ypos = 0 + bdr_s - 20;

    QString   szSLD;
    // 1. text  Distance
    if( speedLimitAheadDistance >= 5 )
    {
      if( speedLimitAheadDistance >= 1000 )
        szSLD.sprintf("%.1fkm", speedLimitAheadDistance * 0.001 );
      else
        szSLD.sprintf("%.0f", speedLimitAheadDistance );

      QColor crFill = QColor(0,0,0,100); // nvgRGBA(255, 255, 255,100);
      
      float fpR[] = {255, 255, 0};
      float fpG[] = {0, 255, 100};
      float fpB[] = {0, 100, 255};
      float xp[] = {100, 300, 500};
      int N = sizeof(xp) / sizeof(xp[0]);
      float x = speedLimitAheadDistance;
      int nR = (int)interp( x, xp,  fpR, N);
      int nG = (int)interp( x, xp,  fpG, N);
      int nB = (int)interp( x, xp,  fpB, N);
      
      crFill = QColor(nR, nG, nB, 200);
      

      int txt_size = int(img_size1*0.8);
      int txt_xpos = img_xpos + 20;  
      int txt_ypos = img_ypos + img_size1 - 15;
      QRect rect( txt_xpos, txt_ypos, txt_size, 60 );  

      p.setPen(QPen(QColor(0xff, 0xff, 0xff, 100), 5));
      p.setBrush( crFill );
      p.drawRoundedRect(rect, 20, 20);
      p.setPen(Qt::NoPen);
    
      p.setPen( QColor(0xff, 0xff, 0xff, 255) ); //QColor(0xff, 0xff, 0xff, alpha));
      p.drawText( rect, Qt::AlignCenter, szSLD );//|Qt::AlignRight));
    }



    // 2. image
    if( traffic_sign  )
    {
      p.drawPixmap(img_xpos , img_ypos, *traffic_sign);

      configFont( p, "Open Sans",  26, "SemiBold");
      szSLD.sprintf("%d", nTrafficSign );
      drawText( p, img_xpos + int(img_size1*0.5), img_ypos+25, szSLD ); 

      if( speedLimit )
      {
        configFont( p, "Open Sans",  85, "SemiBold");
        szSLD.sprintf("%.0f", speedLimit );
        drawText( p, img_xpos + int(img_size1*0.5), img_ypos + int(img_size1*0.79), szSLD, QColor(0,0,0,255) );  
      }       
    }
    else if( nTrafficSign > 0 )
    {
      traffic_sign = &img_img_space;
      p.drawPixmap(img_xpos , img_ypos, *traffic_sign);

      configFont( p, "Open Sans",  50, "SemiBold");
      szSLD.sprintf("%d", nTrafficSign );
      drawText( p, img_xpos + int(img_size1*0.5), img_ypos + int(img_size1*0.5), szSLD, QColor(0,0,0,255) );
    }
}


void OnPaint::ui_draw_navi( QPainter &p ) 
{
  float speedLimit =  scene->liveNaviData.getSpeedLimit();  
  float speedLimitAheadDistance =  scene->liveNaviData.getArrivalDistance(); // getSpeedLimitDistance();  
  float nTrafficSign =  scene->liveNaviData.getSafetySign1();
  int   mapValid =  scene->liveNaviData.getMapValid();


  if( mapValid )
  {
    ui_draw_traffic_sign( p, nTrafficSign, speedLimit, speedLimitAheadDistance );
  }
}

void OnPaint::ui_draw_debug1( QPainter &p ) 
{
  QString text1 = QString::fromStdString(scene->alert.alertTextMsg1);
  QString text2 = QString::fromStdString(scene->alert.alertTextMsg2);
  QString text3 = QString::fromStdString(scene->alert.alertTextMsg3);

  int bb_x = 0;
  int bb_y = 930;
  int bb_w = width();

  QRect rc( bb_x, bb_y, bb_w, 90);
  //p.setPen(QPen(QColor(0xff, 0xff, 0xff, 100), 3)); 
  //p.setPen(Qt::NoPen);
  p.setBrush(QColor(0, 0, 0, 100));
  p.drawRoundedRect(rc, 20, 20); 
  p.setPen( QColor(0xff, 0xff, 0xff, 255) ); 

  QTextOption  textOpt =  QTextOption( Qt::AlignLeft );
  configFont( p, "Open Sans",  40, "Regular");


  p.drawText( QRect(bb_x, 0, bb_w, 42), text1, textOpt );
  p.drawText( QRect(bb_x, bb_y, bb_w, 42), text2, textOpt );
  p.drawText( QRect(bb_x, bb_y+45, bb_w, 42), text3, textOpt );


 // text3.sprintf("BF:%.1f   RL:%.1f°", scene->scr.accel_prob[0], scene->scr.accel_prob[1] );
 // p.drawText( QRect(bb_x, 900, bb_w, 42), text3, textOpt );
}



void OnPaint::ui_main_navi( QPainter &p ) 
{
  ui_draw_navi( p );

  ui_draw_debug1( p );
}
