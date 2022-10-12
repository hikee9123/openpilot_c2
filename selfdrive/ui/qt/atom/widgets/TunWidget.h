

#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QTimer>
#include <QWidget>

#include "selfdrive/ui/qt/widgets/controls.h"
#include "selfdrive/ui/ui.h"

//#include "selfdrive/ui/qt/atom/widgets/TunWidget.h"

#include "selfdrive/ui/qt/atom/TuningPanel.h"


class CTunWidget : public QFrame 
{
  Q_OBJECT

public:
  explicit CTunWidget( TuningPanel *panel, QWidget *parent = 0);
  ~CTunWidget();

  enum TunType {
    LAT_PID = 0,
    LAT_INDI,
    LAT_LQR,
    LAT_TOROUE,
    LAT_HYBRID,
    LAT_MULTI,
    LAT_DEFAULT,
    LAT_ALL,
  };

private:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;
  void closeSettings();

public slots:  
  void refresh();


private:
  void  FramePID(QVBoxLayout *parent=nullptr);
  void  FrameINDI(QVBoxLayout *parent=nullptr);
  void  FrameLQR( int nMode=0, QVBoxLayout *parent=nullptr);
  void  FrameTOROUE( int nMode=0, QVBoxLayout *parent=nullptr);
  void  FrameHYBRID(QVBoxLayout *parent=nullptr);

  void  FrameHide( int nID = -1 );
  void  FrameShow( int nID );
  QVBoxLayout *CreateBoxLayout( int nID );


  
 private:
  Params params; 
  QLabel *icon_label;
  QPixmap  pix_plus;
  QPixmap  pix_minus;


  QVBoxLayout *main_layout;
  QPushButton *title_label;
  QHBoxLayout *hlayout;
  QLabel *description = nullptr;  

  TuningPanel  *m_pPanel;
  QPushButton  *method_label;
  int    m_nMethod;
  int    m_bShow;





   QFrame *m_pChildFrame[LAT_ALL];


};

