#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QMap>
#include <QStack>
#include <QDialog>
#include "embeddedsubiface.h"

class QTimer;
class QMutex;
class QLabel;

namespace ew {
    class TaskPanel;
class WaitDialog : public QDialog, public ew::EmbeddedSubIFace
{
    Q_OBJECT

    friend class TaskPanel;
    friend class WaitController;

protected:
    enum IndicatorStyle {StyleRect, StyleEllipse, StyleArc};

    explicit WaitDialog(QWidget *parent = 0);
    void  paintEvent(QPaintEvent *);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    QWidget* getWidget();

    void setIndicatorStyle(IndicatorStyle);
    void           setColor(QColor color);
    IndicatorStyle indicatorStyle() const;
    void           setText(QString str);


private slots:
    void rotate();

protected slots:
    /**
     * @brief Функция установки состояния
     * @param txt - текст состояния
     */
    void slotSetCurrentState(quint64 stateId, QString txt);

    /**
     * @brief Функция удаляет текущее состояние
     */
    void slotRemoveCurrentState(quint64 stateId);

private:
    QPixmap generatePixmap(int sideLength);
    void    drawRectStyle(QPainter *painter);
    void    drawEllipseStyle(QPainter *painter);
    void    drawArcStyle(QPainter *painter);

        quint64 m_currentStateId;
    QTimer *m_timer;
    int m_startAngle;
    QString m_text;
    IndicatorStyle m_style;
    QColor m_fillColor;
};

/**
 * @brief Класс-контроллер для панели задач. Thread safe
 */
class WaitController : public QObject
{
    Q_OBJECT
    QStack<quint64> m_states;
    QMap<quint64, QString> m_stateString;
    //TaskPanel *m_pPanel;
    WaitDialog *m_pWaitDialog;
    QMutex *m_pMutex;

protected:
    WaitController();

    void init();
public:
    ~WaitController();
    static WaitController *instance();

    /**
     * @brief Функция установка состояния
     * @param txt - текст состояния
     * @return - id состояния
     */
    quint64 setState(QString txt);

    /**
     * @brief Удаление состояния из стека
     * @param stateId - id удаляемого состояния
     */
    void removeState(quint64 stateId);

    /**
     * @brief Возврат текста состояния
     * @param id - id состояния
     * @return текст состояния
     */
    QString getStateText(quint64 id);

    /**
     * @brief Возврат активного состояния
     * @return id активного состояния
     */
    quint64 getCurrentState();


protected slots:
    /**
     * @brief Возврат первого свободного id состояния
     * @return свободный id
     */
    quint64 getEmptyStateId();

    /**
     * @brief Слот вызывается при уничтожении объекта панели
     */
    void slotPanelDestroyed();

signals:
    /**
     * @brief Сигнал для установки состояния в виджете
     * @param stateId - id состояния
     * @param txt - текст состояния
     */
    void signalSetCurrentState(quint64 stateId, QString txt);

    /**
     * @brief Сигнал удаления состояния из панели
     * @param stateId - id состояния
     */
    void signalRemoveCurrentState(quint64 stateId);
};


/**
 * @brief Класс виджета-панели главного окна приложения
 */
class TaskPanel : public QWidget
{
    Q_OBJECT

    friend class WaitController;

    WaitDialog *m_pWaitWidget;
    QLabel *m_pLabel;
    quint64 m_currentId;

protected:
    explicit TaskPanel(QWidget *parent = 0);

protected slots:
    /**
     * @brief Функция установки состояния
     * @param txt - текст состояния
     */
    void slotSetCurrentState(quint64 id, QString txt);

    /**
     * @brief Функция удаляет текущее состояние
     */
    void slotRemoveCurrentState(quint64 id);

private:
signals:
public slots:
};
}


#endif // WAITDIALOG_H
