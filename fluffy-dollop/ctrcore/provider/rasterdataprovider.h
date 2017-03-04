#ifndef RASTERDATAPROVIDER_H
#define RASTERDATAPROVIDER_H

#include "abstractdataprovider.h"
#include "t_image.h"

//#include <dpf/geo/geoimage_object.h>
#include <QSharedPointer>
#include <QUrl>
#include <QIcon>
#include <QStack>
#include <QDateTime>

namespace data_system
{

//class RasterThread;

// тип привязки провайдера
//enum class MatchingType
//{
//    None,       // не привязан
//    Location,   // привязан к позиции съемки
//    Corners     // привязан по углам
//};

// структура описывает позицию съёмки изображения, данные берутся из EXIF изображения.
//struct Location
//{
//    Location(double _GPSLatitude, double _GPSLongitude, double _GPSImgDirection = 0, int _orientation = 1)
//        : GPSLatitude(_GPSLatitude), GPSLongitude(_GPSLongitude), GPSImgDirection(_GPSImgDirection), orientation(_orientation)
//    {}

//    Location(const Location& other)
//    : GPSLatitude(other.GPSLatitude), GPSLongitude(other.GPSLongitude), GPSImgDirection(other.GPSImgDirection), orientation(other.orientation)
//    {}

//    Location() : GPSLatitude(0), GPSLongitude(0), GPSImgDirection(0), orientation(0)
//    {}

//    double GPSLatitude;
//    double GPSLongitude;
//    double GPSImgDirection;
//    int orientation;
//};

class RasterDataProvider : public AbstractDataProvider
{
Q_OBJECT

//friend class RasterThread;

QIODevice *m_device;

public:
    /**
     * @brief Конструктор
     */
    RasterDataProvider();

    /**
     * @brief Деструктор
     */
    virtual ~RasterDataProvider();

    /**
     * @brief open Метод, предназначенный для конвертирования исходного файла изображения в требуемый формат
     * @param url - Имя нового файла
     * @param type - Тип файла в формате GDAL
     * @return  true/false
     */
    virtual bool toFormat(const QUrl &url, const QString &type) = 0;

    /**
     * @brief  Метод для получения прямоугольника занимаемого данными
     * @return прямоугольник
     */
    virtual QRect boundingRect() = 0;

    /**
     * @brief  Возвращает гео-точки boundingRect()
     * @return
     */
    virtual QRectF spatialRect() = 0;

    virtual QPolygonF spatialPolygon() = 0;
    /**
     * @brief  Метод, возвращает количество изображений, которые в данный момент загружены в провайдер
     * @return количество растров провайдера
     */
    virtual uint32_t rasterCount() = 0;

    /**
     * @brief  Метод, предназначенный для получения фрагмента сформированных данных
     * @param img ссылка на объект изображения, куда будет записано готовое изображение
     * @param source параметры запрашиваемых данных
     * @param dest размер выходных данных (по умолчанию задается из source)
     * @param async возможность асинхронного получения данных
     * @param band номер растра в контейнере растров данного провайдера
     *
     * Если задана возможность асинхронного получения данных, то в качестве возвращаемого значения
     * будет возвращен идентификатор задачи на чтение данных, а данные можно будет прочитать по сигналу
     * dataReady(int). Возвращаемый идентификатор в данном случае будет больше нуля.
     * В случае если задан асинхронный режим работы, но StoragePolicy позволяет прочесть данные из кэша (т.е. фактически синхронно),
     * то провайдер ведет себя как Sync - т.е. возвращает 0 и данные можно получить следующей инструкцией.
     *
     * @return 0 - в случае, если задано синхронное чтение данных,@n
     * число > 0 - в случае, если задано асинхронное чтение данных,@n
     * -1 - в случае ошибки (при любом типе получения данных)
     */
    virtual int readData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize(), bool async = false, uint32_t band = 1) = 0;

    /**
     * @brief  Метод, предназначенный для записи данных
     * @param img ссылка на объект изображения, откуда будет записано готовое изображение
     * @param source параметры запрашиваемого фрагмента изображения
     * @param dest размер выходных данных (по умолчанию задается из source)
     * @param band номер растра в контейнере растров данного провайдера
     *
     * @return -1 - в случае ошибки записи
     */
    virtual int writeData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize(), uint32_t band = 1) = 0;

    /**
     * @brief  Метод, предназначен для получения фрагмента "сырого" изображения
     * @param img ссылка на объект изображения, куда будет записано готовое изображение
     * @param depth значение глубины отдачи "сырого" изображения см. enum RawDepth
     * @param x координата левого верхнего угла видимого изображения
     * @param y координата левого верхнего угла видимого изображения
     * @param w ширина видимого изображения
     * @param h высота видимого изображения
     * @return -1 - в случае ошибки
     */
    virtual int readRawData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize()) = 0;

    /**
     * @brief  Метод, предназначенный для получения растра по гео-точкам
     * @param img ссылка на объект изображения
     * @param source гео-точки запрашиваемого фрагмента
     * @param dest пиксельный размер итогового изображения
     * @param band номер растра в контейнере растров данного провайдера
     * @return -1 - в случае ошибки
     */
    virtual int readSpatialData(image_types::TImage &img, QRectF source, QRect dest = QRect(), uint32_t band = 1) = 0;

    /**
     * @brief  Метод, предназначенный для получения растра по гео-точкам
     * @param img ссылка на объект изображения
     * @param epsgCode проекция, в которой задан "source" и в которой будет сформирован результат
     * @param source гео-точки запрашиваемого фрагмента
     * @param dest пиксельный размер итогового изображения
     * @param band номер растра в контейнере растров данного провайдера
     * @return -1 - в случае ошибки
     */
    virtual int readSpatialData(image_types::TImage &img, int epsgCode, QRectF source, QSize dest = QSize(), uint32_t band = 1) = 0;

    /**
     * @brief  Метод, предназначенный для получения растра по гео-точкам
     * @param img ссылка на объект изображения
     * @param proj4code проекция, в которой задан "source" и в которой будет сформирован результат
     * @param source гео-точки запрашиваемого фрагмента
     * @param dest пиксельный размер итогового изображения
     * @param band номер растра в контейнере растров данного провайдера
     * @return -1 - в случае ошибки
     */
    virtual int readSpatialData(image_types::TImage &img, const QString & proj4code, QRectF source, QSize dest = QSize(), uint32_t band = 1) = 0;

    /**
     * @brief  Метод, предназначен для регистрации приема поточных данных (пр. видео)
     * @param img ссылка на объект изображения, куда будет производиться запись данных
     * @param lock мьютекс, который будет лочить эту память
     *
     * Новые данные будут доступны по сигналу dataReady(int), который будет вызван с идентификатором
     * полученным в качестве возвращаемого значения
     *
     * @return -1 в случае ошибки
     */
    virtual int registerDataListener(image_types::TImage &img, QMutex &lock) = 0;

    /**
     * @brief  Метод, предназанченный для удаления регистрации приема поточных данных
     * @param num идентификатор, полученный при вызове registerDataListener(image_types::TImage&, QMutex&)
     */
    virtual void destroyDataListener(int num) = 0;

    /**
     * @brief  Метод, предназначенный для возвращения интервала опроса данного провайдера, в случае если он поточный
     * @return значение интервала опроса
     */
    virtual qreal updateInterval() = 0;

    /**
     * @brief Метод, предназначенный для прерывания получения данных в асинхронном режиме
     */
    virtual void abort() = 0;

    /**
     * @brief Метод, предназанченный для прерывания конкретной задачи
     * @param task
     */
    virtual void abort(int task) = 0;

    /**
     * @brief geoImageObjectPtr - функция переопределена из базового класса,
     * всегда возвращается объект, указывающий на полное изображение
     * @return укзатель на объект, или NULL, если объект не может быть создан
     */
//    virtual QSharedPointer<dpf::io::GeoImageObject> geoImageObjectPtr();

    /**
     * @brief Метод, предназанченный для установки интерполяции данных при последующем считывании
     */
//    virtual void readDataWithInterpolation(bool on_off) {}

    /**
     * @brief setOriginalGeoData - метод вызывается при нахождении привязки вне файла изображения (jpg-подобные изображения).
     * @param geoData - данные привязки.
     */
//    virtual void setOriginalGeoData(dpf::geo::GeoData* geoData) {}
//    virtual void setOriginalLocation(Location location) {}

    /**
     * @brief undo - откат на верхнее значение стека "m_ProviderUndoActions",
     *               если стек пуст то файл переключается на m_originalUrl, привязка переключается на m_OriginalGeoData.
     * @return - успешность выполнения отката.
     */
    virtual bool undo() {return false;}

    /**
     * @brief setMatchingData - метод устанавливает новую привязку
     * @param geoData - привязка
     * @param addCurrentGeoDataToStack - при TRUE флаг добавляет текущее значение привязки в стек отмены операций провайдера,
     *                                   при FALSE необходим во время ручной привязки, чтоб не запоминать все промежуточные значения привязки.
     */
//    virtual void setMatchingData(dpf::geo::GeoData* geoData, bool addCurrentGeoDataToStack = true) {}

//    virtual void setLocation(Location location) {}
//    virtual bool getLocation(Location & location) {return false;}

//    virtual void setObjreprRasterId(uint64_t id) {m_objreprRasterId = id;}
//    virtual uint64_t getObjreprRasterId() {return m_objreprRasterId;}

//    virtual data_system::MatchingType getMatchingType() {return MatchingType::None;}

    virtual bool providerChanged() {return false;} // были ли произведены с провайдером изменения ?

    //    virtual bool rollbackMatchingData() {return false;}
    //    QList<ProviderHistory> virtual getProviderHistory() {return m_ProviderHistory.toList();}
    //    virtual bool clearProviderHistory() {return false;}
    //    virtual bool saveMatchingData() {return false;}

protected:
//    uint64_t m_objreprRasterId;
//    dpf::geo::GeoData* m_originalGeoData;
//    Location * m_originalLocation;
    QUrl m_originalUrl;

private:
    /**
     * @brief Метод, предназначенный для загрузки данных в рабочий буфер относительно StoragePolicy
     */
    virtual void loadRaster() = 0;

    /**
     * @brief Метод, предназначенный для сохранения данных в рабочий буфер относительно StoragePolicy
     */
    virtual void saveRaster() = 0;

signals:
    /**
     * @brief  Сигнал для определения мометна чтения данных
     * @param listener идентификатор по которому необходимо прочесть данные
     */
    void dataReady(int listener);
};
}


#endif // RASTERDATAPROVIDER_H
