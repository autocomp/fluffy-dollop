#ifndef TDPROVIDER_H
#define TDPROVIDER_H

#include "rasterdataprovider.h"

namespace data_system
{

class TileDataProvider : public RasterDataProvider
{
    Q_OBJECT

public:

    /**
     *  Перечисление описывает возможных заказчиков тайловых данных
     */
    enum TmsTaskSender
    {
        TmsMapper2D,
        TmsMapper3D,
        AltitudeController
    };

    /**
     *  Перечисление описывает возможные варианты результата загрузки данных
     */
    enum Result
    {
        Loaded,     // данные успешно загружены
        NotFound,   // данные не найдены
        Aborted     // загрузка данных была отменена извне (например загрузка исключена из очереди при певышении лимита размера)
    };

    /**
     * @brief Конструктор класса
     */
    TileDataProvider();

    /**
     * @brief Деструктор класса
     */
    virtual ~TileDataProvider();

    /**
     * @brief registerSource метод предназначенный для регистрации отправителя запросов на получение тайлов от провайдера.
     * @param taskSender тип отправителя запросов согласно перечислению "TileDataProvider::TmsTaskSender"
     * @return присвоенный идентификатор отправителя запросов, необходимо запомнить и использовать в методе "tile" и прочих.
     */
    virtual uint registerSource(TileDataProvider::TmsTaskSender taskSender) = 0;

    /**
     * @brief unregisterSource метод предназначенный для удаления отправителя запросов.
     * @param source идентификатор удаляемого отправителя запросов.
     * @return успешность удаления из системы.
     */
    virtual bool unregisterSource(uint sourceId) = 0;

    /**
     * @brief tile метод предназначенный для записи тайла по заданным координатам в матрицу image_types::TImage.
     * @param img
     * @param x
     * @param y
     * @param z
     * @param source идентификатор источника запроса, необходим для распаралеливания очередей(стеков) загрузки от разных источников в пуле загрузчиков.
     */
    virtual int tile(image_types::TImage &img, int x, int y, int z, uint sourceId) = 0;

    virtual void abortAllTaskFromSource(uint sourceId) = 0;

    virtual void  setMinZoomLevel(uint minZlevel) = 0;
    virtual void  setMaxZoomLevel(uint maxZlevel) = 0;
    virtual void  setRetryLimit(uint retryLimit) = 0;
    virtual void  setTimeoute(uint timeouteSec) = 0;
    virtual void  setScaleForBottomLevels(bool on_off) = 0;
    virtual void  setLoadersLimit(int loadersLimit) = 0;
    uint virtual minZoomLevel()const = 0;
    uint virtual maxZoomLevel()const = 0;
    uint virtual retryLimit()const = 0;
    uint virtual timeouteSec()const = 0;
    bool virtual scaleForBottomLevels()const = 0;
    int virtual loadersLimit()const = 0;

signals:
    /**
     * @brief  Сигнал отправляемый по завершению процесса загрузки данных
     * @param taskid идентификатор задачи, по которому необходимо прочесть данные
     * @param sourceId идентификатор источника запроса
     * @param result результат загрузки
     */
    void tmsDataReady(int taskid, uint sourceId, TileDataProvider::Result result);
};
}

#endif // TILEDATAPROVIDER_H
