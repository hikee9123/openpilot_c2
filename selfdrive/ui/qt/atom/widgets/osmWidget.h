

#pragma once

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QStackedLayout>
#include <QTimer>
#include <QWidget>


#include "selfdrive/ui/qt/atom/TuningPanel.h"
#include "selfdrive/ui/qt/widgets/groupWidget.h"



class CosmWidget : public CGroupWidget 
{
  Q_OBJECT

public:
  explicit CosmWidget( TuningPanel *panel );

  enum emType {
    TP_NONE = 0,
    TP_USES,
    TP_ALL,
  };  

 private:
  TuningPanel  *m_pPanel;
  QPushButton  *method_label;
  int    m_nMethod;
  Params params;
  
  void  FrameOSM(QVBoxLayout *parent=nullptr);

  

public slots:  
  virtual void refresh(int nID = 0);  
};



class CWidgetosmConfig : public CGroupWidget 
{
  Q_OBJECT

public:
  explicit CWidgetosmConfig( TuningPanel *panel );

  enum emType {
    TP_NONE = 0,
    TP_USES,
    TP_ALL,
  };  

 private:
  TuningPanel  *m_pPanel;
  QPushButton  *method_label;
  int    m_nMethod;
  Params params;
  
  void  FrameOSM(QVBoxLayout *parent=nullptr);

  

public slots:  
  virtual void refresh(int nID = 0);  
};



class CWidgetosmNodesData : public CGroupWidget 
{
  Q_OBJECT

public:
  explicit CWidgetosmNodesData( TuningPanel *panel );

  enum emType {
    TP_NONE = 0,
    TP_USES,
    TP_ALL,
  };  

 private:
  TuningPanel  *m_pPanel;
  QPushButton  *method_label;
  int    m_nMethod;
  Params params;
  
  void  FrameOSM(QVBoxLayout *parent=nullptr);

  

public slots:  
  virtual void refresh(int nID = 0);  
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
