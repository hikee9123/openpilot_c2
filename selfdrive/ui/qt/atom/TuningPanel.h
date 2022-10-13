#pragma once



#include <QWidget>
#include <QFrame>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QButtonGroup>
#include <QScrollArea>
#include <QStackedWidget>

#include "selfdrive/ui/ui.h"
#include "selfdrive/ui/qt/widgets/controls.h"



class TuningPanel : public QFrame {
  Q_OBJECT

public:
  explicit TuningPanel(QWidget *parent = nullptr);
  ~TuningPanel();

public:
  int    m_nCommand;
  PubMaster *pm = nullptr;

public: 
  void  ConfirmButton(QVBoxLayout *parent, int version = 1 );


public:

};
