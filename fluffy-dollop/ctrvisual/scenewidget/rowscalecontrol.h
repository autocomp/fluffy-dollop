#ifndef ROWSCALECONTROL_H
#define ROWSCALECONTROL_H

#include <QWidget>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>

class RowScaleControl : public QWidget
{
    Q_OBJECT
    QHBoxLayout * m_pLayout;
    QSpinBox * m_pSpinBox;
    QPushButton * m_pOkButton;
    QComboBox *m_pComboBox;


    enum ScaleNums
    {
        SN_CM,
        SN_DM,
        SN_M,
        SN_KM
    };

public:
    explicit RowScaleControl(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *k);
    void focusOutEvent(QFocusEvent *e);
    void mousePressEvent(QMouseEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

signals:

    /**
     * @brief Сигнал на установку текущего состояния масшатаба.
     * @param meters
     */
    void signalSetCurrentScaleMeters(qreal meters);

private slots:
    void slotOkButtonClicked();
    void slotEditScaleFinished();
    
};

#endif // ROWSCALECONTROL_H
