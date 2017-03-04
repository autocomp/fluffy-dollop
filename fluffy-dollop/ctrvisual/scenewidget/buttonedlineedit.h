#ifndef BUTTONEDLINEEDIT_H
#define BUTTONEDLINEEDIT_H

#include <QLineEdit>
#include <QToolButton>
#include <QStyle>
#include <QKeyEvent>

class QToolButton;

#include <QLineEdit>

class QToolButton;


class ButtonedLineEdit : public QLineEdit {
    Q_OBJECT

  public:
    // Constructors and destructors
    ButtonedLineEdit(QWidget *parent = 0);
    ~ButtonedLineEdit();

  public slots:
    void setClearButtonEnabled(bool enable);
    void setEnabled(bool enable);
    void setReadOnly(bool read_only);

  protected slots:
    void onTextChanged(const QString &new_text);
    void slotFind();

  protected:
    void resizeEvent(QResizeEvent *event);
    int frameWidth() const;

  private:
    QToolButton *m_clearButton;
    bool m_clearButtonEnabled;

signals:
    void sigFind(QString str);
};

#endif // BUTTONEDLINEEDIT_H



