#ifndef ABSTRACTVIDEODATAPROVIDER_H
#define ABSTRACTVIDEODATAPROVIDER_H

#include "t_image.h"
//#include "rasterdataprovider.h"
#include "abstractdataprovider.h"

namespace data_system
{
class FrameInfo
{
public:
    FrameInfo() :
        data(0),
 		dataSize(0),
        frameNumber(0),
        pts(0),
        readDelay(0),
        dts(0),
        w(0),
        h(0),
        colorDepth(0),
		durationTime(0)
    {}
    /**
     * @brief data - Данные изображения.
     */
    char *data;

	/**
     * @brief dataSize - Размер данных.
     */
    uint dataSize;

    /**
     * @brief frameNumber - Номер кадра.
     */
    int frameNumber;

    /**
     * @brief pts - Presentation time stamp. Временная метка, показывающая
     * очерёдность когда кадр должен быть показан. Значения строго упорядочены.
     */
    double pts;

    /**
     * @brief readDelay - Задержка чтения и декодирования (в миллисекундах).
     */
    uint readDelay;

    /**
     * @brief dts - Decoding time stamp. Временная метка, показывающая
     * очерёдность когда кадр должен быть декодирован. Значения не строго упорядочены.
     */
    qulonglong dts;

    /**
     * @brief currentTime - Строка с временем (в формате чч:мм:сс).
     */
    QString currentTime;

    /**
     * @brief w - Ширина кадра.
     */
    uint w;

    /**
     * @brief h - Высота кадра.
     */
    uint h;

    /**
     * @brief colorDepth - Глубина цвета.
     */
    uint colorDepth;

	/**
     * @brief duration - Длительность.
     */
    qlonglong durationTime;
};

//enum use_lib
//{
//    gstreamer = 0,
//    libav = 1,//not supported yet!
//    ffmpeg = 2,//not supported yet!
//    libvlc = 3 //not supported yet!
//};

class AbstractVideoDataProvider : public AbstractDataProvider
{
Q_OBJECT

public:
    /**
     * @brief Напрвление проигрывания.
     */
    enum PlayDirection
    {
        Forward = 0,
        Backward
    } playDirection;

    /**
     * @brief Соотношение сторон.
     */
    enum AspectRatio
    {
        KeepAspectRatio = 0,
        NoKeepAspectRatio
    };

    /**
     * @brief Конструктор
     */
    AbstractVideoDataProvider();

    /**
     * @brief Деструктор
     */
    virtual ~AbstractVideoDataProvider();

    /**
     * @brief Задаёт провайдер источник видеоданных.
     * @param pSource - Указатель на провайдер источника видеоданных (используется для записи в файл)
     */
    virtual void setSourceProvider(AbstractVideoDataProvider *pSource);

    /**
     * @brief  Метод для инициирования проигрывания видео
     * @return true - если видео возможно начать проигрывать false - если видео нельзя проиграть
     */
    virtual bool play() = 0;

    /**
     * @brief  Метод для инициирования проигрывания в обратном порядке видео.
     * @return true - если видео возможно начать проигрывать false - если видео нельзя проиграть.
     */
    virtual bool playBackward() = 0;

    /**
     * @brief  Метод для остановки проигрывания видео и возврата к первому видеокадру
     */
    virtual void stop() = 0;

    /**
     * @brief  Метод для остановки проигрывания видео на текущем видеокадре.
     */
    virtual void pause() = 0;

    /**
     * @brief  Метод для записи видео в файл.
     * @param url - путь к выходному файлу.
     */
    virtual bool record() = 0;

    /**
     * @brief  Метод для перехода на следующий видеокадр.
     * @return true - если возможно перейти на следующий видеокадр false - если нет.
     */
    virtual bool frameForward() = 0;

    /**
     * @brief  Метод для перехода на предидущий видеокадр.
     * @return true - если возможно перейти на предидущий видеокадр false - если нет.
     */
    virtual bool frameBackward() = 0;

    /**
     * @brief  Метод для перехода на произвольный видеокадр.
     * @param frameNum номет кадра на который надо перейти.
     * @return true - если возможно перейти на предидущий видеокадр false - если нет.
     */
    virtual bool seek(int frameNum) = 0;

    /**
     * @brief  Метод для установки скорости проигрования.
     * @param fps количество кадров всекунду.
     */
    virtual bool setFPS(uint fps) = 0;

    /**
     * @brief  Метод для получения текущей скорости проигрования.
     * @return Количество кадров в секунду.
     */
    virtual quint16 getFPS() const = 0;

    /**
     * @brief  Метод, предназначенный для установки метаданных.
     * @param key ключ
     * @param value данные
     */
    virtual void setMetaData(const QString &key, const QVariant &value) = 0;

    /**
     * @brief  Метод, предназначенный для получения класса свойств прочитанных данных
     * @return объект класса совйств
     */
    virtual const MetaData &metaData() = 0;

    /**
     * @brief  Метод, предназначенный для изменения размеров видео (сначала необходимо вызвать метод setResizeByWindow(true);)
     * @param w ширина видеофпейма
     * @param h высота видеофрейма
     */
    virtual void setFrameSize(int w, int h, AspectRatio ar = KeepAspectRatio) = 0;

    virtual bool readData(image_types::TImage &tImg) = 0;

    virtual void setPlayDirection(PlayDirection playDirection) = 0;

    //int readData(image_types::TImage &img, QRect source  = QRect(), QSize dest = QSize(), bool async  = false, uint32_t band = 1) = 0;

    //int writeData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize(), uint32_t band = 1) = 0;

    bool open(const QUrl &url, WorkMode mode) = 0;

    WorkMode workMode() const;

    QStringList accessType() const;

    //QStringList resourceType() const = 0;

    bool toFormat(const QUrl &url, const QString &type);

    virtual QRect boundingRect();
    QRectF spatialRect();
    uint32_t rasterCount();
    int readRawData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize());
    int readSpatialData(image_types::TImage &img, QRectF source, QRect dest = QRect(), uint32_t band = 1);
    int readSpatialData(image_types::TImage &img, const QString & proj4code, QRectF source, QSize dest = QSize(), uint32_t band = 1);
    int readSpatialData(image_types::TImage &img, int epsgCode, QRectF source, QSize dest = QSize(), uint32_t band = 1);
    int registerDataListener(image_types::TImage &img, QMutex &lock);
    void destroyDataListener(int num);
    qreal updateInterval();
    void abort();
    void abort(int task);
    void loadRaster();
    void saveRaster();
    QMap<QString, QStringList> outKeys() const;
    QList<InParam> outParameters(const QString &) const;
    StoragePolicy storagePolicy() const;
    bool setStoragePolicy(StoragePolicy);
    ProviderType providerType() const;
    bool isFinished();
    //use_lib current_using_lib;

signals:
    void currentDTS(qulonglong);
    void currentTime(QString);
    void getFrame(FrameInfo *pFrame);
    void finished();
};
}


#endif // ABSTRACTVIDEODATAPROVIDER_H
