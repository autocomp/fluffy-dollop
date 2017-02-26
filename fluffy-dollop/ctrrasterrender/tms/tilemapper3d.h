#ifndef TILEMAPPER3D_H
#define TILEMAPPER3D_H


///**
// * @brief The TileMapper class Класс мапинга координат
// * Класс обеспечивает доступ к тайловому, растровому и ТМС провайдерам через тайловый интерфейс
// */
//class TileMapper3D : public QObject
//{
//    Q_OBJECT

//private:
//    RasterDataProvider* raster_provider;
//    TMSDataProvider *tms_provider;
//    const QSize tileSize;
//    const QSize tilesCount;
//    QMap<uint, TempStruct> m_Map;// контейнер для ожидания загрузки тайлов провайдером
//    QSet<QString> m_Set;
//    uint mapperSourceId;

//    int loadTile(image_types::TImage *img, int x, int y, int z);
//    QPointF geoToScene(const QPointF& coordinateIn);
//    QPointF sceneToGeo(const QPointF& point);
//    QRectF getTileExtend(int x, int y, int z);
//    QPolygonF getBoundingPolygone();

//public:
//    /**
//     * @brief Конструктор класса
//     * @param provider укзатель на провайдер, с которым будет работать маппер
//     */
//    TileMapper3D(RasterDataProvider* provider, QSize tileSize = QSize(256,256), QSize tilesCount = QSize(2,1));
//    ~TileMapper3D();
//    RasterDataProvider* provider() const;
//    bool isTMSProviderMapper();

//public slots:
//    void loadTile(int x, int y, int z, QString returnKey);
//    void abort(const QString &returnKey);
//    void abortAllTasks();

//private slots:
//    void slotRasterDataReady(int task);
//    void slotDataReady(int task, uint sourceId, TileDataProvider::Result result);

//signals:
//    //  Сигнал устарел !!! Срочно перейти на использование "dataReady" !!!
//    //  При получении проверять img.isValid(), если false - значит тайл не загружен.
//    void signal_paintTile(const QString& returnKey, QImage& img);
//    //  При получении проверять знаение "result" !!!
//    void dataReady(const QString& returnKey, QImage& img, TileDataProvider::Result result);
//};

#endif
