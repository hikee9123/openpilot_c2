

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


class CSteerWidget : public QFrame 
{
  Q_OBJECT

public:
  explicit CSteerWidget(TuningPanel *panel,QWidget *parent = 0);
  ~CSteerWidget();

private:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;

public slots:  
  void refresh();


private:
  void  FrameSmooth(QWidget *parent);
  void  FrameNormal(QWidget *parent);

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
  int    m_nSelect;
  int    m_bShow;

   QFrame *m_pChildFrame1;
   QFrame *m_pChildFrame2;
};


class CLaneWidget : public QFrame 
{
  Q_OBJECT

public:
  explicit CLaneWidget(TuningPanel *panel, QWidget *parent = 0);
  ~CLaneWidget();

private:
  void showEvent(QShowEvent *event) override;
  void hideEvent(QHideEvent *event) override;

public slots:  
  void refresh();


private:
  void  FrameCamera(QWidget *parent);
  void  FrameLane(QWidget *parent);


 private:
  Params params; 
  QLabel  *icon_label;
  QPixmap  pix_plus;
  QPixmap  pix_minus;


  QVBoxLayout *main_layout;
  QPushButton *title_label;
  QHBoxLayout *hlayout;
  QLabel *description = nullptr;  

  TuningPanel  *m_pPanel;
  QPushButton  *method_label;
  int    m_nSelect;
  int    m_bShow;


   QFrame *m_pChildFrame1;
   QFrame *m_pChildFrame2;
};




class CNaviSelect : public CGroupWidget 
{
  Q_OBJECT

public:
  explicit CNaviSelect( TuningPanel *panel );

  enum emType {
    TP_NONE = 0,
    TP_MAPPY,
    TP_INAVI,
    TP_ALL,
  };  

 private:
  TuningPanel  *m_pPanel;
  QPushButton  *method_label;
  int    m_nMethod;
  Params params;
  
  void  FrameMappy(QVBoxLayout *parent=nullptr);
  void  FrameINavi(QVBoxLayout *parent=nullptr);
  

public slots:  
  virtual void refresh(int nID = 0);  
};
