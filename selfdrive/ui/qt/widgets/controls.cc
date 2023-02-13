#include "selfdrive/ui/qt/widgets/controls.h"

#include <QPainter>
#include <QStyleOption>

QFrame *horizontal_line(QWidget *parent) {
  QFrame *line = new QFrame(parent);
  line->setFrameShape(QFrame::StyledPanel);
  line->setStyleSheet(R"(
    margin-left: 40px;
    margin-right: 40px;
    border-width: 1px;
    border-bottom-style: solid;
    border-color: gray;
  )");
  line->setFixedHeight(2);
  return line;
}

AbstractControl::AbstractControl(const QString &title, const QString &desc, const QString &icon, QWidget *parent) : QFrame(parent) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

  main_layout = new QVBoxLayout(this);
  main_layout->setMargin(0);

  hlayout = new QHBoxLayout;
  hlayout->setMargin(0);
  hlayout->setSpacing(20);

  // left icon
   icon_label = new QLabel();  
  if (!icon.isEmpty()) {
    icon_pixmap = QPixmap(icon).scaledToWidth(80, Qt::SmoothTransformation);
    icon_label->setPixmap(icon_pixmap);
    icon_label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    hlayout->addWidget(icon_label);
  }

  // title
  title_label = new QPushButton(title);
  title_label->setFixedHeight(100);
  title_label->setStyleSheet("font-size: 50px; font-weight: 300; text-align: left");
  hlayout->addWidget(title_label);

  // value next to control button
  value = new ElidedLabel();
  value->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  value->setStyleSheet("color: #aaaaaa");
  hlayout->addWidget(value);

  main_layout->addLayout(hlayout);

  // description
  //if (!desc.isEmpty()) {
  description = new QLabel(desc);
  description->setContentsMargins(40, 20, 40, 20);
  description->setStyleSheet("font-size: 40px; color: grey");
  description->setWordWrap(true);
  description->setVisible(false);
  main_layout->addWidget(description);

  connect(title_label, &QPushButton::clicked, [=]() {
    if (!description->isVisible()) {
      emit showDescriptionEvent();
    }

    if (!description->text().isEmpty()) {
      description->setVisible(!description->isVisible());
    }
  });
  //}
  main_layout->addStretch();
}

void AbstractControl::hideEvent(QHideEvent *e) {
  if(description != nullptr) {
    description->hide();
  }
}

// controls

ButtonControl::ButtonControl(const QString &title, const QString &text, const QString &desc, QWidget *parent) : AbstractControl(title, desc, "", parent) {
  btn.setText(text);
  btn.setStyleSheet(R"(
    QPushButton {
      padding: 0;
      border-radius: 50px;
      font-size: 35px;
      font-weight: 500;
      color: #E4E4E4;
      background-color: #393939;
    }
    QPushButton:pressed {
      background-color: #4a4a4a;
    }
    QPushButton:disabled {
      color: #33E4E4E4;
    }
  )");
  btn.setFixedSize(250, 100);
  QObject::connect(&btn, &QPushButton::clicked, this, &ButtonControl::clicked);
  hlayout->addWidget(&btn);
}

// ElidedLabel

ElidedLabel::ElidedLabel(QWidget *parent) : ElidedLabel({}, parent) {}

ElidedLabel::ElidedLabel(const QString &text, QWidget *parent) : QLabel(text.trimmed(), parent) {
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setMinimumWidth(1);
}

void ElidedLabel::resizeEvent(QResizeEvent* event) {
  QLabel::resizeEvent(event);
  lastText_ = elidedText_ = "";
}

void ElidedLabel::paintEvent(QPaintEvent *event) {
  const QString curText = text();
  if (curText != lastText_) {
    elidedText_ = fontMetrics().elidedText(curText, Qt::ElideRight, contentsRect().width());
    lastText_ = curText;
  }

  QPainter painter(this);
  drawFrame(&painter);
  QStyleOption opt;
  opt.initFrom(this);
  style()->drawItemText(&painter, contentsRect(), alignment(), opt.palette, isEnabled(), elidedText_, foregroundRole());
}

ClickableWidget::ClickableWidget(QWidget *parent) : QWidget(parent) { }

void ClickableWidget::mouseReleaseEvent(QMouseEvent *event) {
  emit clicked();
}

// Fix stylesheets
void ClickableWidget::paintEvent(QPaintEvent *) {
  QStyleOption opt;
  opt.init(this);
  QPainter p(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}



//  MenuControl

MenuControl::MenuControl( const QString &str_param, const QString &title, const QString &desc, const QString &icon, QWidget *parent ) 
  : AbstractControl( title, desc, icon, parent )
{
  m_nDelta = 10;
  m_nMax = 100;
  m_nMin = 0;
  m_nValue = 0;



  label.setAlignment(Qt::AlignVCenter|Qt::AlignRight);
  label.setStyleSheet( R"(
    color: #e0e879;
    font-size: 70px;
    font-weight: 300;    
    background-color: transparent;    
  )");

  hlayout->addWidget(&label);


  auto str = QString::fromStdString( params.get( str_param.toStdString() ) );
  float value = str.toDouble();
  m_dValue = value;

  // 1. selected
  btnsel.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnsel.setFixedSize(150, 100);
  //btnsel.setText( values );
  hlayout->addWidget(&btnsel);

  QObject::connect(&btnsel, &QPushButton::clicked, [=]() {

    if( m_defDelta >= 10 )
    {
      if(  m_nDelta <= 0.105 )  m_nDelta = 1;
      else if(  m_nDelta <= 1.05 )  m_nDelta = 10;
      else if(  m_nDelta <= 10.05 )  m_nDelta = 100;
      else if(  m_nDelta <= 100.05 )  m_nDelta = 1000;
      else m_nDelta = 0.1;
    }
    else if( m_defDelta >= 1 )
    {
      if(  m_nDelta <= 0.015 )  m_nDelta = 0.1;
      else if(  m_nDelta <= 0.105 )  m_nDelta = 1;
      else if(  m_nDelta <= 1.05 )  m_nDelta = 10;
      else if(  m_nDelta <= 10.05 )  m_nDelta = 100;      
      else m_nDelta = 0.01;
    }      
    else if( m_defDelta >= 0.1 )
    {
      if(  m_nDelta <= 0.0015 )  m_nDelta = 0.01;
      else if(  m_nDelta <= 0.015 )  m_nDelta = 0.1;
      else if(  m_nDelta <= 0.105 )  m_nDelta = 1;
      else if(  m_nDelta <= 1.05 )  m_nDelta = 10;
      else m_nDelta = 0.001;
    }    
    else if( m_defDelta >= 0.01 )
    {
      if(  m_nDelta <= 0.00015 )  m_nDelta = 0.001;
      else if(  m_nDelta <= 0.0015 )  m_nDelta = 0.01;
      else if(  m_nDelta <= 0.015 )  m_nDelta = 0.1;
      else if(  m_nDelta <= 0.105 )  m_nDelta = 1;
      else m_nDelta = 0.0001;
    }       
    else 
    {
      if(  m_nDelta <= 0.000015 )  m_nDelta = 0.0001;
      else if(  m_nDelta <= 0.00015 )  m_nDelta = 0.001;
      else if(  m_nDelta <= 0.0015 )  m_nDelta = 0.01;
      else if(  m_nDelta <= 0.015 )  m_nDelta = 0.1;
      else m_nDelta = 0.00001;
    }


    refresh();
  });

  // 2. minus
  btnminus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");

  btnminus.setFixedSize(150, 100);
  btnminus.setText("－");
  hlayout->addWidget(&btnminus);
  
  QObject::connect(&btnminus, &QPushButton::clicked, [=]() {
    m_dValue -= m_nDelta;
    if (m_dValue < m_nMin) {
      m_dValue = m_nMin;
    }
    QString values = QString::number(m_dValue);
    params.put( str_param.toStdString() , values.toStdString());

    refresh();
  });

  // 3. plus
  btnplus.setStyleSheet(R"(
    padding: 0;
    border-radius: 50px;
    font-size: 35px;
    font-weight: 500;
    color: #E4E4E4;
    background-color: #393939;
  )");
  btnplus.setFixedSize(150, 100);
  btnplus.setText("＋");
  hlayout->addWidget(&btnplus);

  QObject::connect(&btnplus, &QPushButton::clicked, [=]() {
    m_dValue += m_nDelta;
    if (m_dValue > m_nMax) {
      m_dValue = m_nMax;
    }
    QString values = QString::number(m_dValue);
    params.put( str_param.toStdString(), values.toStdString());
    refresh();
  });

  refresh();
}

void MenuControl::SetControl( float nMin, float nMax, float nDelta )  
{ 
  m_defDelta = nDelta;
  m_nDelta = nDelta; 
  m_nMin = nMin;  
  m_nMax = nMax;
  refresh();
}

void MenuControl::refresh() 
{
  QString values = QString::number( m_dValue );

  int count = m_strList.size();
  if( count > 0 )
  {
    int  nMenu = m_dValue;
    
    if( 0 <= nMenu && nMenu <= count )
      values = m_strList[nMenu];
  }
  else if( !m_strValue.isEmpty() )
  {
    if( m_nValue == m_dValue  )
        values = m_strValue;
  }
  label.setText( values );

  values = QString::number(m_nDelta);
  btnsel.setText( values );  
}

void MenuControl::SetString( const QString strList )
{
  m_strList = strList.split(",");
}

void MenuControl::SetString( float nValue, const QString str )
{
  m_nValue = nValue;
  m_strValue = str;
}
