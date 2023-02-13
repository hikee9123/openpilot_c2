#pragma once

#include <QStackedLayout>
#include <QWidget>

#include "selfdrive/common/util.h"
#include "selfdrive/ui/qt/widgets/cameraview.h"
#include "selfdrive/ui/ui.h"

#include "selfdrive/ui/qt/atom/paint.h"
#include "selfdrive/ui/qt/atom/dashcam.h"
// ***** onroad widgets *****
class OnroadAlerts : public QWidget {
  Q_OBJECT

public:
  OnroadAlerts(QWidget *parent = 0) : QWidget(parent) {};
  void updateAlert(const Alert &a, const QColor &color);

protected:
  void paintEvent(QPaintEvent*) override;

private:
  QColor bg;
  Alert alert = {};
};


class ExperimentalButton : public QPushButton {
  Q_OBJECT

public:
  explicit ExperimentalButton(QWidget *parent = 0);
  void updateState(const UIState &s);

private:
  void paintEvent(QPaintEvent *event) override;

  Params params;
  QPixmap engage_img;
  QPixmap experimental_img;
};

// container window for the NVG UI
class NvgWindow : public CameraViewWidget {
  Q_OBJECT
  Q_PROPERTY(QString speedData MEMBER speedData);
  Q_PROPERTY(QString speedUnit MEMBER speedUnit);
  Q_PROPERTY(float setSpeed MEMBER setSpeed);
  Q_PROPERTY(float speedLimit MEMBER speedLimit);  
  Q_PROPERTY(bool is_cruise_set MEMBER is_cruise_set);
  Q_PROPERTY(bool engageable MEMBER engageable);
  Q_PROPERTY(bool dmActive MEMBER dmActive);
  Q_PROPERTY(bool hideDM MEMBER hideDM);
  Q_PROPERTY(int status MEMBER status);

public:
  explicit NvgWindow(VisionStreamType type, QWidget* parent = 0);
  void updateState(const UIState &s);

private:
  void drawIcon(QPainter &p, int x, int y, QPixmap &img, QBrush bg, float opacity);
  void drawText(QPainter &p, int x, int y, const QString &text, int alpha = 255);
  void drawCurrentSpeed(QPainter &p, int x, int y);

  ExperimentalButton *experimental_btn;
  QPixmap engage_img;
  QPixmap dm_img;
  const int radius = 192;
  const int img_size = (radius / 2) * 1.5;
  QString speedData;
  QString speedUnit;
  float  setSpeed;
  float  speedLimit;

  bool is_cruise_set = false;
  bool engageable = false;
  bool dmActive = false;
  bool hideDM = false;
  bool accActive = false;

  int status = STATUS_DISENGAGED;



private:
  int  m_nBrakeStatus;
  float m_gasVal;


private:
  void ui_draw_line(QPainter &painter, const line_vertices_data &vd);

protected:
  void paintGL() override;
  void initializeGL() override;
  void showEvent(QShowEvent *event) override;
  void updateFrameMat(int w, int h) override;
  void drawLaneLines(QPainter &painter, const UIState *s);
  void drawLead(QPainter &painter, const cereal::ModelDataV2::LeadDataV3::Reader &lead_data, const QPointF &vd);
  void drawHud(QPainter &p);
  inline QColor redColor(int alpha = 255) { return QColor(201, 34, 49, alpha); }
  inline QColor whiteColor(int alpha = 255) { return QColor(255, 255, 255, alpha); }
  inline QColor blackColor(int alpha = 255) { return QColor(0, 0, 0, alpha); }
  
  double prev_draw_t = 0;
  FirstOrderFilter fps_filter;
};

// container for all onroad widgets
class OnroadWindow : public QWidget {
  Q_OBJECT

public:
  OnroadWindow(QWidget* parent = 0);
  bool isMapVisible() const { return map && map->isVisible(); }

private:
  void paintEvent(QPaintEvent *event);
  void mousePressEvent(QMouseEvent* e) override;
  OnroadAlerts *alerts;
  NvgWindow *nvg;
  QColor bg = bg_colors[STATUS_DISENGAGED];
  QWidget *map = nullptr;
  QHBoxLayout* split;

  // atom
  OnPaint *m_pPaint = nullptr;
  OnDashCam *m_pDashCam = nullptr;

private slots:
  void offroadTransition(bool offroad);
  void updateState(const UIState &s);
};
