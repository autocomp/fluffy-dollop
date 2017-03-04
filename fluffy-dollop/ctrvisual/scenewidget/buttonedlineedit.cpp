

#include "buttonedlineedit.h"
#include <QVBoxLayout>

ButtonedLineEdit::ButtonedLineEdit(QWidget *parent) :
    QLineEdit(parent) {
  m_clearButton = new QToolButton(this);

  QVBoxLayout * lt = new QVBoxLayout(this);
  lt->addWidget(m_clearButton);
  lt->setAlignment(m_clearButton, Qt::AlignRight | Qt::AlignVCenter );
  lt->setMargin(0);

  lt->setSpacing(0);

  int frame_width = frameWidth();
  m_clearButton->setIcon(QIcon::fromTheme("edit-clear",
                                          QIcon(":/26_delete.png")));
  m_clearButton->setIconSize(QSize(sizeHint().height() -4 ,
                                   sizeHint().height() -6 ));
  m_clearButton->setCursor(Qt::ArrowCursor);
  //m_clearButton->setStyleSheet("QToolButton { border: none; padding: 0px; }");
  m_clearButton->setToolTip(tr("find"));
  m_clearButton->hide();
 // m_clearButton->setFixedSize(this->width() - 2, this->width() - 2);
  m_clearButtonEnabled = true;

  connect(m_clearButton, SIGNAL(clicked()), this, SLOT(slotFind()));
  connect(this, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
  //connect(this, SIGNAL(editingFinished()), this, SLOT(onTextChanged(QString))

//  setStyleSheet(QString("ButtonedLineEdit { padding-right: %1px; }").arg(m_clearButton->sizeHint().width() +
//                                                                 frame_width + 4));

}

ButtonedLineEdit::~ButtonedLineEdit() {
  delete m_clearButton;
}

void ButtonedLineEdit::onTextChanged(const QString &new_text) {
  if (isReadOnly() == false && isEnabled() == true && m_clearButtonEnabled == true) {
    m_clearButton->setVisible(new_text.isEmpty() == false);
  }
}

void ButtonedLineEdit::slotFind()
{
    if(m_clearButton->isVisible())
    {
        emit sigFind(text());
    }
}

void ButtonedLineEdit::setClearButtonEnabled(bool enable) {
  m_clearButtonEnabled = enable;
}

void ButtonedLineEdit::setEnabled(bool enable) {
  QLineEdit::setEnabled(enable);
  onTextChanged(text());
}

void ButtonedLineEdit::setReadOnly(bool read_only) {
  QLineEdit::setReadOnly(read_only);
  onTextChanged(text());
}

int ButtonedLineEdit::frameWidth() const {
  return style()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, this);
}

void ButtonedLineEdit::resizeEvent(QResizeEvent *event) {
  Q_UNUSED(event);

  // Place clear button correctly, according to size of line edit.
  QSize sz = m_clearButton->sizeHint();
  m_clearButton->move(rect().right()  - sz.width(),
                      rect().bottom() - sz.height() + frameWidth());
}



