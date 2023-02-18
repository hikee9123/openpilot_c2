#include "selfdrive/ui/qt/atom/paint.h"

#include <cmath>

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QConicalGradient>
#include <QPen>

#include "selfdrive/common/timing.h"
#include "selfdrive/ui/qt/util.h"

#include "system/logcatd/traffic_sign.h"



// OnroadHud
OnPaint::OnPaint(QWidget *parent) : QWidget(parent) 
{
  m_param.bbh_left = 0;
  m_param.bbh_right = 0;
  m_param.cpuPerc = 0;
  m_param.cpuTemp = 0;
  m_param.batteryTemp = 0;


  m_nOldSec = 0;

  state = uiState();
  scene = &(state->scene);

  img_compass= QPixmap("../assets/addon/Image/img_compass.png").scaled(img_size_compass, img_size_compass, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_direction= QPixmap("../assets/addon/Image/img_direction.png").scaled(img_size_compass, img_size_compass, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  img_tire_pressure= QPixmap("../assets/addon/Image/img_tire_pressure.png").scaled(90, 90, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  //img_rpm1 = QPixmap("../assets/addon/Image/RPM11.png").scaled(img_size_rpm, img_size_rpm, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  //img_rpm2 = QPixmap("../assets/addon/Image/RPM22.png").scaled(img_size_rpm/2, img_size_rpm/2, Qt::KeepAspectRatio, Qt::SmoothTransformation);

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




  connect(this, &OnPaint::valueChanged, [=] { update(); });


  scene->scr.autoFocus = get_param("OpkrAutoFocus");
  scene->scr.brightness_off = get_param("OpkrUIBrightnessOff");
  scene->scr.autoScreenOff = get_param("OpkrAutoScreenOff");
  scene->scr.brightness = get_param("OpkrUIBrightness");
  scene->scr.nTime = scene->scr.autoScreenOff * 60 * UI_FREQ;
}


int OnPaint::get_time()
{
  int iRet;
  struct timeval tv;
  int seconds = 0;

  iRet = gettimeofday(&tv, NULL);
  if (iRet == 0)
  {
    seconds = (int)tv.tv_sec;
  }
  return seconds;
}

int OnPaint::get_param( const std::string &key )
{
    auto str = QString::fromStdString(Params().get( key ));
    int value = str.toInt();

    return value;
}


void OnPaint::updateState(const UIState &s)
{
  enginRpm = s.scene.scr.enginrpm; 
  setProperty("enginRpm", enginRpm );

  if( s.scene.started )
  {
    auto deviceState = s.scene.deviceState;

    m_param.batteryTemp = deviceState.getBatteryTempCDEPRECATED();
    m_param.cpuPerc = deviceState.getCpuUsagePercent()[0];

    if( m_param.cpuPerc < 0) m_param.cpuPerc = 0;
    else if( m_param.cpuPerc > 100) m_param.cpuPerc = 100;


    auto  maxCpuTemp = deviceState.getCpuTempC();
      m_param.cpuTemp = maxCpuTemp[0];   
  }
  else
  {
    m_param.cpuPerc = 0;
    m_param.cpuTemp = 0;
    m_param.batteryTemp = 0;
  }

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


    update(); 



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


  bb_ui_draw_UI( p );
  ui_main_navi( p );

  if( scene->scr.IsCalibraionGridViewToggle )
  {
    ui_draw_grid( p );
  }
}


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

    if( m_param.cpuTemp > 100 )  m_param.cpuTemp = 0;

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

  
   //add battery temperature
   lab_color = QColor(255, 255, 255, 200);
  if( true )
  {
    QColor val_color = QColor(255, 255, 255, 200);

    if( m_param.batteryTemp > 200 )  m_param.batteryTemp = 0;

    val_color = get_color( (int) m_param.batteryTemp, 50, 40 );

    uint16_t fan_speed = scene->deviceState.getFanSpeedPercentDesired();

    // temp is alway in C * 1000
    val_str.sprintf("%.1f", m_param.batteryTemp );
    uom_str.sprintf("%.0f", fan_speed * 0.001);        
    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "BAT TEMP",
        bb_rx, bb_ry, bb_uom_dx,
        val_color, lab_color, uom_color,
        value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }



  //add grey panda GPS accuracy
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
  if( false )
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

  // GPS
  if( false )
  {
   // float angleSteersDes = scene->controls_state.getSteeringAngleDesiredDegDEPRECATED();  
    uom_color = QColor(255, 255, 255, 200);
    QColor val_color = QColor(255, 255, 255, 200);

    val_str.sprintf("%.1f", scene->scr.accel_prob[0]);  // BF
    uom_str.sprintf("%.1f", scene->scr.accel_prob[1]);  // RL

    bb_h +=bb_ui_draw_measure(p,  val_str, uom_str, "GRADIENT",
      bb_rx, bb_ry, bb_uom_dx,
      val_color, lab_color, uom_color,
      value_fontSize, label_fontSize, uom_fontSize );
    bb_ry = bb_y + bb_h;
  }

  
  if( true )
  {
    float fEngineRpm = enginRpm;// scene->car_state.getEngineRpm();
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



void OnPaint::bb_draw_rpm(QPainter &p, int compass_x, int compass_y )
{
 // auto   gps_ext = scene->gpsLocationExternal;
 // float  bearingUblox = gps_ext.getBearingDeg();
 float fEngineRpm = enginRpm;//   enginRpm;// scene->scr.enginrpm;

  //fEngineRpm = 3000;

  fEngineRpm *= 0.045;

  if( fEngineRpm <= 0 ) return;


/*
  if( enginRpm < 2000 )
      p.setPen( QPen( QColor(0,255,0,200), 30) );
  else if( enginRpm < 3000 )
      p.setPen( QPen( QColor(255,255,0,200), 35 ) );
  else
      p.setPen( QPen( QColor(255,0,0,200), 35) );
*/
  int nStartDegree = -140;
 // p.drawArc(compass_x, compass_y, 500, 200, nStartDegree * 16, nStartDegree - fEngineRpm * 16);


//QRectF rect = QRectF(compass_x, compass_y, 500, 200);


//int barWidth = 20;

p.setRenderHint(QPainter::Antialiasing, true);
QConicalGradient gradient;
gradient.setCenter( compass_x+200, compass_y+100 );
gradient.setAngle(-90);
gradient.setColorAt(0, QColor(255, 44, 0));
gradient.setColorAt(1, QColor(0, 255, 100));
//gradient.setColorAt(1, QColor(255, 255, 0));


int  m_width = 20;
QPen pen(QBrush(gradient), m_width);
pen.setCapStyle(Qt::RoundCap);
p.setPen( pen );
//p.setPen(QPen(Qt::black, barWidth, Qt::SolidLine,Qt::RoundCap));

p.drawArc(compass_x, compass_y, 500, 200, nStartDegree * 16, nStartDegree - fEngineRpm * 16);
//p.drawArc(barWidth/2, barWidth/2, this->width() - barWidth, this->height() - barWidth,
//                90*16, progressInDegrees*-16);}
  
  p.setPen(Qt::NoPen);  
}

void OnPaint::bb_ui_draw_UI(QPainter &p)
{
  const int bb_dml_w = 180;
  const int bb_dml_x = (0 + bdr_s);
  const int bb_dml_y = (0 + bdr_s) + 220;

  const int bb_dmr_w = 180;
  const int bb_dmr_x = 0 + state->fb_w - bb_dmr_w - bdr_s;
  const int bb_dmr_y = (0 + bdr_s) + 220;

 // const int bb_dml_x = bb_dmr_x - bb_dmr_w - 10;  

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
    const int compass_x = state->fb_w / 2 - 150;
    const int compass_y = state->fb_h - 150;
    bb_draw_compass( p, compass_x, compass_y );
  }

  // 4. rpm
  if( true )
  {
    const int rpm_x = state->fb_w / 2 - 250;
    const int rpm_y = 10; 
    bb_draw_rpm( p, rpm_x, rpm_y );
  }
}
//BB END: functions added for the display of various itemsapType

//////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Nevi
//

/*
this is navigation code by OPKR, and thank you to the OPKR developer.
I love OPKR code.
*/

void OnPaint::ui_draw_traffic_sign( QPainter &p ) 
{
  float speedLimit =  scene->liveNaviData.getSpeedLimit();  
  float speedLimitAheadDistance =  scene->liveNaviData.getArrivalDistance(); // getSpeedLimitDistance();  
  int nTrafficSign1 =  scene->liveNaviData.getSafetySign1();
  int nTrafficSign2 =  scene->liveNaviData.getSafetySign2();

  int nMapType =  scene->liveNaviData.getMapType();
  


  QPixmap  *traffic_sign = NULL;
  int  nTrafficSign =  0;

  if( nMapType == MAP_MAPPY )
  {
    nTrafficSign = int( nTrafficSign1 );

    if( nTrafficSign == TS_BEND_ROAD ) traffic_sign = &img_traf_turn;  
    else if( nTrafficSign == TS_VARIABLE ) traffic_sign = &img_speed_var;
    else if( nTrafficSign == TS_BUS_ONLY ) traffic_sign = &img_bus_only; 
    else if( nTrafficSign == TS_BUMP_ROAD ) traffic_sign = &img_speed_bump; 
    else if( nTrafficSign == TS_SCHOOL_ZONE1 ) traffic_sign = &img_school_zone;
    else if( nTrafficSign == TS_SCHOOL_ZONE2 ) traffic_sign = &img_school_zone; 
    else if( nTrafficSign == TS_CURVE_RIGHT ) traffic_sign = &img_curve_right; 
    else if( nTrafficSign == TS_CURVE_LEFT ) traffic_sign = &img_curve_left; 
    else if( nTrafficSign == TS_NARROW_ROAD ) traffic_sign = &img_narrow_road; 
    else if( nTrafficSign == TS_RAIL_ROAD ) traffic_sign = &img_rail_road;  
    else if( nTrafficSign == TS_PARK_CRACKDOWN ) traffic_sign = &img_park_crackdown;
    else if( nTrafficSign == TS_LANE_CHANGE1 ) traffic_sign = &img_img_space; 
    else if( nTrafficSign == TS_ANE_CHANGE2 ) traffic_sign = &img_img_space; 
    else if( nTrafficSign == TS_LOAD_OVER ) traffic_sign = &img_img_space; 
    else if( nTrafficSign == TS_TRAFFIC_INFO ) traffic_sign = &img_img_space; 
   // else if( nTrafficSign == TS_OVERTRAK ) traffic_sign = &img_overtrack;  
    else if( nTrafficSign == TS_SHOULDER  ) traffic_sign = &img_img_space; 
    else if( nTrafficSign == TS_LOAD_POOR  ) traffic_sign = &img_img_space; 

    

    else if( nTrafficSign == TS_CAMERA2_BUS ) traffic_sign = &img_bus_only; 
    else if( speedLimit ) 
    {
      if( nTrafficSign == TS_INTERVAL || nTrafficSign == TS_INTERVAL2 )  
      {
        traffic_sign = &img_section;
      }
      else if( nTrafficSign == TS_CAMERA1  )
      {
        if( speedLimit == 30 ) traffic_sign = &img_school_zone;
        else traffic_sign = &img_camera;
      }
      else
      {
        traffic_sign = &img_speed;
      }
    }
  }
  else if( nMapType == MAP_iNAVI )
  {
    nTrafficSign = int( nTrafficSign1 );


    if( nTrafficSign == TC_BUS_ONLY1 ) traffic_sign = &img_bus_only; 
    else if( nTrafficSign == TC_BUS_ONLY2 ) traffic_sign = &img_bus_only; 
    else if( nTrafficSign == TR_SPEED_BUMP ) traffic_sign = &img_speed_bump; 

    else if( nTrafficSign == TC_SCHOOL_ZONE1 ) traffic_sign = &img_school_zone;
    else if( nTrafficSign == TC_SCHOOL_ZONE2 ) traffic_sign = &img_school_zone; 
    else if( nTrafficSign == TC_TRAFFIC_INFO ) traffic_sign = &img_img_space; 
    else if( nTrafficSign == TC_PARK_CRACKDOWN ) traffic_sign = &img_park_crackdown;
    else if( nTrafficSign == TC_NO_LANE_CHANGE ) traffic_sign = &img_img_space; 


    else if( nTrafficSign2 == TR_RAIL_CROSS ) traffic_sign = &img_rail_road;  
    else if( nTrafficSign2 == TR_SPEED_BUMP ) traffic_sign = &img_speed_bump; 
    

    else if( speedLimit ) 
    {
      if( nTrafficSign == TC_INTERVAL1 || nTrafficSign == TC_INTERVAL2 )  
      {
        traffic_sign = &img_section;
      }
      else if( nTrafficSign == TC_CAMERA2 || nTrafficSign == TC_CAMERA3  )
      {
        traffic_sign = &img_camera;
      }
      else
      {
        traffic_sign = &img_speed;
      }
    }

  }
  else
  {
    return;
  }



    int img_size1 = img_size;   // 472
    int img_xpos = 0 + bdr_s + 184 + 20;
    int img_ypos = 0 + bdr_s - 20;

    QString   szSLD;
    // 1. text  Distance
    if( speedLimitAheadDistance >= 5 )
    {
      if( speedLimitAheadDistance >= 1000 )
        szSLD.sprintf("%.1fk", speedLimitAheadDistance * 0.001 );
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
      int txt_ypos = img_ypos + img_size1 - 5;
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
  int   mapValid =  scene->liveNaviData.getMapValid();


  if( mapValid )
  {
    ui_draw_traffic_sign( p );
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



  // stop line status display
  QString text4;
  if ( scene->stopLine.used && scene->stopLine.x > 1.0 ) {
    text3.sprintf("Stop line:%.1f   X:%.1f Y:%.1f Z:%.1f", scene->stopLine.Prob, scene->stopLine.x, scene->stopLine.y, scene->stopLine.z );
    p.drawText( QRect(bb_x, 700, bb_w, 42), text3, textOpt );
  }  

 // text3.sprintf("BF:%.1f   RL:%.1f°", scene->scr.accel_prob[0], scene->scr.accel_prob[1] );
 // p.drawText( QRect(bb_x, 900, bb_w, 42), text3, textOpt );
}

void OnPaint::ui_view_tunning( QPainter &p ) 
{
  p.save();

  int bb_x = 250;
  int bb_y = 300;


  QString text4;
  int  nYPos = bb_y;
  int  nGap = 80;




  auto car_params =  scene->car_params;
  auto lateralTuning = car_params.getLateralTuning();
  auto tunName =  lateralTuning.which();
  // 0.PID, 1:INDI, 2:LQR, 3:Torque, 4:hybrid, 5:multi
 
  if( tunName == 3 )
  {
    auto torque  = lateralTuning.getTorque();
    auto max_lat_acc = car_params.getMaxLateralAccel();    

    text4 = "Torque";                                         p.drawText( bb_x, nYPos+=nGap, text4 );

    configFont( p, "Open Sans",  80, "Regular");    
    text4.sprintf("LA = %.2f", max_lat_acc );                 p.drawText( bb_x, nYPos+=nGap, text4 );
    text4.sprintf("FC = %.5f", torque.getFriction() );        p.drawText( bb_x, nYPos+=nGap, text4 );
  }
  else if( tunName == 2 )
  {
    auto lqr  = lateralTuning.getLqr();
    text4 = "lqr";                                          p.drawText( bb_x, nYPos+=nGap, text4 );
    configFont( p, "Open Sans",  80, "Regular");
    text4.sprintf("scale = %f", lqr.getScale() );           p.drawText( bb_x, nYPos+=nGap, text4 );
    text4.sprintf("ki = %f", lqr.getKi() );                 p.drawText( bb_x, nYPos+=nGap, text4 );   
    text4.sprintf("DcGain = %f", lqr.getDcGain() );         p.drawText( bb_x, nYPos+=nGap, text4 );   
  }



  p.restore();  

}


void OnPaint::ui_view_normal( QPainter &p ) 
{
  p.save();

  int bb_x = 250;
  int bb_y = 300;


  QString text4;
  int  nYPos = bb_y;
  int  nGap = 80;


  auto car_params =  scene->car_params;
  auto laneParam = car_params.getLaneParam();


  auto cameraOffsetAdj = laneParam.getCameraOffsetAdj();    
  auto pathOffsetAdj = laneParam.getPathOffsetAdj();    
  auto leftLaneOffset = laneParam.getLeftLaneOffset();    
  auto rightLaneOffset = laneParam.getRightLaneOffset();    

  text4 = "Offset";                                    p.drawText( bb_x, nYPos+=nGap, text4 );

  configFont( p, "Open Sans",  80, "Regular");    
  text4.sprintf("Camera = %.1f", cameraOffsetAdj );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("Path = %.1f", pathOffsetAdj );        p.drawText( bb_x, nYPos+=nGap, text4 );

  text4.sprintf("Left Lane = %.1f", leftLaneOffset );    p.drawText( bb_x, nYPos+=nGap, text4 );
  text4.sprintf("right Lane = %.1f", rightLaneOffset );  p.drawText( bb_x, nYPos+=nGap, text4 );


  p.restore();  

}

void OnPaint::ui_tunning_data( QPainter &p ) 
{
  uint64_t nSec = get_time();
  uint64_t nDelta = nSec - m_nOldSec;

  int nCmd = scene->update_data.getCommand();

  if( m_nOldSec <= 0 )
  {
    m_nOldSec = nSec;
    nDelta = 0;
  }
  else  if( m_nOldCmmand != nCmd ) 
  {
    m_nOldSec = nSec; 
    m_nOldCmmand = nCmd;
  }

 

  if( nDelta > 30 )     // 30 sec
  {
      m_view_tunning_data = 0;
      return;
  }
  else
  {
     m_view_tunning_data = 1;
  }

  
  int nVersion = scene->update_data.getVersion();
  if( nVersion <= 1 )
    ui_view_tunning( p );
  else if( nVersion == 2 )
    ui_view_normal( p );
}


void OnPaint::ui_main_navi( QPainter &p ) 
{
  ui_draw_navi( p );

  if( scene->scr.IsCalibraionGridViewToggle )  return;

  ui_draw_debug1( p );

  ui_tunning_data( p );


}




// grid line by opkr for mounting device appropriate position  BF:Back and Forth Angle, RL:Right and Left Angle
void OnPaint::ui_draw_grid( QPainter &p  ) 
{
  int x_center = state->fb_w/2;
  int y_center = state->fb_h/2;
  int nGap = 0;

   p.setPen( QColor(220, 220, 220, 100) ); 

  for (int i = 0; i < 5; i++) {
    nGap = i * 200;
    p.drawLine( x_center + nGap, 0, x_center+ nGap , state->fb_h);  // line
    p.drawLine( x_center - nGap, 0, x_center- nGap , state->fb_h); 
  }

  for (int i = 0; i < 5; i++) {
    nGap = i * 200;
    p.drawLine( 0, y_center + nGap, state->fb_w, y_center + nGap); 
    p.drawLine( 0, y_center - nGap, state->fb_w, y_center - nGap); 
  }


  QString text4;
  configFont( p, "Open Sans",  80, "SemiBold");
  text4.sprintf("BF %.2f  RL %.2f", scene->scr.accel_prob[0], scene->scr.accel_prob[1] );
  drawText( p, x_center, y_center, text4 ); 
}
