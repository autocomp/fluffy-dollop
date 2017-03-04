#ifndef ROWSCALE_H
#define ROWSCALE_H

#include <QLabel>
#include <QWidget>
#include <QGraphicsView>
#include <QString>
#include <QVBoxLayout>
#include "rowscaleinner.h"
/**
 * @brief Класс вывода измеренного расстояния/рисования масштаба
 */
class RowScale : public QWidget
{
    Q_OBJECT

    RowScaleInner * m_pRowScaleInner;
    QSize m_settingsPanelSize;
    double m_scale;
    int m_tileSize, m_minTiles;
    int m_zoom;
    int m_minShownZoom;
    double m_distanceMeters;
    bool m_showDistValue;
    QString m_distStr;
    QPixmap pm;
    qreal m_userSetMeters;
    qreal m_mInPixel;

    int m_scaleWidth;

    QString m_outTextUp, m_outTextDown;
    qreal m_outValueUp, m_outValueDown;
    qreal m_downScalePixelLen, m_upScalePixelLen;
    /*допустимые стандартные размерности для вывода на экран в метрах*/
    QVector<qreal> m_defaultDim;
    const int m_hMargin = 10;

    QLabel *m_pTextUpLabel;
    QLabel *m_pTextDownLabel;

public:
    explicit RowScale(QWidget *parent = 0);
    ~RowScale();

    /**
     * @brief Установка измеренного значения расстояния
     * @param расстояния в метрах
     */
    void setDistanceValue(double distanceMeters);
    /**
     * @brief Установка расстояния между двумя соседними пикселями:
     * когда меняется зум, линейка должна быть перерисована в соответсвии с текущим масштабом.
     * Данная функция сообщает линейке, какое расстояние(между) между 2мя соседними пикселями вьюхи на текущем уровне зума
     * Расстояние меряется в плоскости расположения линейки. По значению расстояния рисуется линейка с оптимальной шириной.
     * @param метры между 2мя соседними пикселями
     */
    void setInterpixelDistance(qreal meters);
    /**
     * @brief переключает линейку в режим отображения измеренного расстояния
     */
    void switchToMeasureDistance();
    /**
     * @brief переключает линейку в режим шкалы(масштаба)
     */
    void switchToScale();


    /**
     * @brief Получает текущий размер линейки (нижней части) в px
     * @return
     */
    int getScalePxLenght();

    /**
     * @brief Пользовательское задание на отображение верхней части линейки с масштабом meters
     * @param meters
     */
    void setUserSet(qreal meters);
protected:
    //void paintEvent(QPaintEvent * e);
    void mousePressEvent(QMouseEvent *e);

private:
    /** Рисование линейки
     * @brief ;
     */
   // void drawScale();

    /**
     * @brief Расчет и подбор параметров линейки (длины в метрах и пикселях)
     */
    void calcRowScale();


    /**
     * @brief Перевод метров в нормированные единицы(см, дм, м,  км)
     * @param meters
     * @return
     */
    QString convertMetersToNormString(qreal meters);

signals:
    void signalShowScaleControl();
public slots:

 //   void slotScaleChanged(double scale, int m_zoom, int, int);
};

#endif // ROWSCALE_H
