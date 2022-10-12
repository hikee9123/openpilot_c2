#pragma once

#include <QStackedLayout>
#include <QWidget>


#include "selfdrive/ui/ui.h"



class OnPaint : public QWidget 
{
  Q_OBJECT
  Q_PROPERTY(int enginRpm MEMBER enginRpm NOTIFY valueChanged);  



public:
  explicit OnPaint(QWidget *parent);
  void updateState(const UIState &s);

private:
  void    paintEvent(QPaintEvent *event) override;
  void    drawText(QPainter &p, int x, int y, const QString &text, QColor qColor = QColor(255,255,255,255), int nAlign = Qt::AlignCenter );
  QColor  get_color( int nVal, int nRed, int nYellow );
  int     get_time();

private:
  void  ui_tunning_data( QPainter &p );
  void  ui_view_tunning( QPainter &p );
  void  ui_view_normal( QPainter &p );

private:
  UIState  *state;
  UIScene  *scene;

  int   enginRpm = 0;
  int   m_view_tunning_data = 0;


 
  struct _PARAM_
  {
    int   bbh_left;
    int   bbh_right;
    float altitudeUblox;
    float gpsAccuracyUblox;
    float bearingUblox;
    float  batteryTemp;
    float  angleSteers;
    float  angleSteersDes;
    
    int   cpuPerc;
    float cpuTemp; 
    
    cereal::RadarState::LeadData::Reader lead_radar;
    cereal::CarState::Reader car_state;    

  } m_param, m_old;
  

private:
  int   m_nOldCmmand;
  uint64_t   m_nOldSec;
  const int img_size = 200;// (radius / 2) * 1.5;
  const int img_size_compass = 300;

  

  QPixmap img_traf_turn;
  QPixmap img_compass;
  QPixmap img_direction;
  QPixmap img_tire_pressure;

  // speed
  QPixmap img_camera;
  QPixmap img_speed;
  QPixmap img_section;


  QPixmap img_speed_var;
  QPixmap img_img_space;
  QPixmap img_car_left;
  QPixmap img_car_right;
  QPixmap img_speed_bump;
  QPixmap img_bus_only;
  QPixmap img_school_zone;

  QPixmap img_curve_right;
  QPixmap img_curve_left;
  QPixmap img_narrow_road;
  QPixmap img_rail_road;

 // QPixmap img_overtrack;
  QPixmap img_park_crackdown;


  const int img_size_rpm = 300;
  QPixmap img_rpm1;
  QPixmap img_rpm2;


// osm
private:
  struct _OSM__
  {
    int    lastGpsTimestamp;
    int    delta_gpstimestamp;
  } m_osm;
   


// navi
private:
  void   ui_draw_grid( QPainter &p  ) ;
  float  interp( float xv, float xp[], float fp[], int N);
  void   ui_main_navi( QPainter &p );
  void   ui_draw_debug1( QPainter &p );
  void   ui_draw_navi( QPainter &p );
  void   ui_draw_traffic_sign( QPainter &p );
  int    get_param( const std::string &key );
// kegmen
private:

  //void ui_draw_circle_image_rotation(const UIState *s, int center_x, int center_y, int radius, const char *image, NVGcolor color, float img_alpha, float angleSteers = 0);
  int  bb_ui_draw_measure(QPainter &p,  const QString &bb_value, const QString &bb_uom, const QString &bb_label,
    int bb_x, int bb_y, int bb_uom_dx,
    QColor bb_valueColor, QColor bb_labelColor, QColor bb_uomColor,
    int bb_valueFontSize, int bb_labelFontSize, int bb_uomFontSize );


  void bb_ui_draw_measures_right(QPainter &p, int bb_x, int bb_y, int bb_w );
  void bb_ui_draw_measures_left(QPainter &p, int bb_x, int bb_y, int bb_w );

  QColor angleSteersColor( int angleSteers );
  QColor get_tpms_color(float tpms);
  QString get_tpms_text(float tpms);

  void  bb_draw_tpms(QPainter &p, int viz_tpms_x, int viz_tpms_y );
  void  bb_draw_compass(QPainter &p, int compass_x, int compass_y );
  void  bb_draw_rpm(QPainter &p, int compass_x, int compass_y );
  void  bb_ui_draw_UI(QPainter &p);




signals:
  void valueChanged();  
};
