#ifndef COORDBAR_H
#define COORDBAR_H

#include <QLabel>
#include <QComboBox>
#include <QWidget>

//#include <dpf/geo/convertion.h>


/**
 * @brief Панель переключения системы координат
 */
class CoordBar : public QWidget
{
    Q_OBJECT

    QComboBox * m_comboBox;
    QLabel * m_pHeightLabel;
    QLabel * m_pLatitudeLabel;
    QLabel * m_pLongitudeLabel;

    qreal m_curLattitude, m_curLongtitude;
    double m_curHeight;
    QMap<int, QString> m_coordSystems;
    QString m_longStr, m_latStr, m_heightStr;
    bool m_visible;
    bool m_objectSelected ;
    bool m_geoCoord;

//    dpf::geo::AngleConvertion angleConvertion;
//    dpf::geo::Convertion systemConvert;

//    dpf::geo::CoordinateSystem getCoordinateType();
    QString coordToString(double data);

public:
    explicit CoordBar(QWidget *parent = 0);
    ~CoordBar();

    /**
     * @brief setHeightLabelShown Установка отображения высоты
     * @param isShown
     */
    void setHeightLabelShown(bool isShown);

    /**
     * @brief Выводит текущие координаты на панель. Не преобразует!
     * @param lattitude - широта
     * @param longtitude - долгота
     * @param lattLongPrecision - количество знаков после запятой для широты и долготы
     * @param heightMeters  - высота
     */
    void setCurrentCoords(qreal lattitude, qreal longtitude, int lattLongPrecision, double heightMeters);
    void setCurrentCoords(QString lattitude, QString longtitude, double heightMeters);
    void setCurrentCoords(QString lattitude, QString longtitude, QString heightMeters);


    /**
     * @brief Добавляет систему координат
     * @param coordSystemName - имя системы координат(выводится в combo box)
     * @param coordSystemId - индитификатор системы координат - любое число. Оно будет возвращено
     * сигналом currentCoordSystemChanged в случае выбора данной системы
     */
    void addCoordSystem(QString coordSystemName, int coordSystemId);

    /**
     * @brief После вызова на панели координаты обведены белым кругом в случае objSelected = true
     * @param objSelected
     */
    void setObjectSelected(bool objSelected );

    /**
     * @brief Функция возвращает индекс текущей системы координат
     * @return
     */
    int getCurCoordSystem();

    /**
     * @brief Функция устанавливает индекс текущей системы координат
     */
    void setCurCoordSystem(int coordSystemId);

    /**
     * @brief Функция возвращает количество элементов в комбобоксе систем координат
     */
    int itemsCount() { m_comboBox->count(); }

    /**
     * @brief Функция очищает комбобокс систем координат
     */
    void clearComboBox() { m_comboBox->clear(); }

    /**
     * @brief Функция заполняет комбобокс набором пиксельных систем координат
     */
    void initPixelCoordSystems();

    /**
     * @brief Функция заполняет комбобокс набором географических систем координат
     */
    void initGeoCoordSystems();
    void initCCKCoordSystems();
    void clearCoordSystem();
    /**
     * @brief Функция конвертирует координаты из WGS-84(гр) в текущую систему координат и выводит их
     * @param latitude - широта WGS-84(гр)
     * @param longitude - долгота WGS-84(гр)
     * @param height - высота(метры)
     */
    void convertionFromWGS84(double longitude, double latitude, double height);
    void convertionFromWGS84(double longitude, double latitude);

    bool getGeoCoord();
    void setGeoCoord( bool coord );
    void setCCKCoord(double longitude, double latitude, double height);

signals:

    /**
     * @brief Сигнал высылается подписчику в случае переключения пользователем системы координат.
     * Подписчик должен выполнить преобразования и установить(setCurrentCoords) координаты в новой системе
     * @param lattitude - широта текущей точки
     * @param longtitude - долгота текущей точки
     * @param height - высота текущей точки
     * @param currentCoordSystem - текущая система координат, в которую нужно перевести.
     */
    void currentCoordSystemChanged(int currentCoordSystem);

private:
    void setHeightValue(double distanceMeters);

private slots:
    void slotCurrentSystemChangedLocal(int newSystem);
};

#endif // COORDBAR_H
