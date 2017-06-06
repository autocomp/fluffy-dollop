#ifndef BUSTAGS_H
#define BUSTAGS_H

namespace visualize_system {
enum class BusTags
{
    //! Запрос на сохранение временных растров в БД, параметр QList<QVariant>, где QVariant-quint64 (ИД временного объекта в обжрепре).
    SaveTemporaryRasters = 1000000000,
    //! Уведомление на запрос о сохранении временных растров в БД.
    TemporaryRastersSaved,
    //! Уведомление об окончании привязки растра.
    RasterMatchingChanged,
    //! Запрос на загрузку растров из БД.
    LoadRastersFromDB,
    //! Включение монопольного режима работы приложения, параметр - QString (имя компонента-монополиста).
    DisableComponent,
    //! Выключение монопольного режима работы приложения, параметр - QString (имя компонента-монополиста).
    EnableComponent,
    //! Старт\окончание сшивки\групповой перепривязки в потоке, параметр - bool (true - старт работы потока, false - окончание).
    SticherInThread,
    //! Запуск\окончание ручной перепривязки провайдера на гео-сцене, параметр - quint64 (>0 - ИД провайдера для запуска перепривязки, ==0 - окончание перепривязки).
    MatchProvider,
    //! Уведомление о том что пользователь закрыл виджет ручной перепривязки провайдера, параметр - QString (не используется).
    MatchingProviderWidgetClosed,
    //! Запрос на сохранение растров в БД, параметр QList<QVariant>, где QVariant-quint64 (ИД провайдера в подсистеме визуализации).
    SaveRastersInImagectrlplugin,
    //! Запрос на открытие растров в плагине галерее изображений пиксельного визуализатора, параметр QList<QVariant>, где первый параметр-ИД пиксельного визуализатора(uint), все последующие-ИД провайдера(uint).
    OpenRastersInImageGallery,
    //! Запрос на скроллирование карты на провайдера (без изменения зума), параметр - quint64 (ИД провайдера).
    ScrollMapToProvider,
    //! Создание уведомления об окончании сшивки, вызывается если результат сшивки отфильтрован, параметр - quint64 (ИД провайдера).
    SetStichingResultToNotificationPlugin,
    //! Запрос на открытие каталога с изображениями в плагине галерее изображений пиксельного визуализатора, , параметр QList<QVariant>, где первый параметр-ИД пиксельного визуализатора(uint), второй параметр - путь к каталогу (QString).
    OpenFolderInImageGallery,
    //! Запуск\окончание, параметр - quint64 (>0 - ИД object, ==0 - окончание).
    EditAreaGeometry,
    //! параметр - quint64 -> ==0 - abort, >0 - type (defect=1, foto=2, foto360=3);
    SetMarkPosition,
    //! Запуск\окончание, параметр - quint64 (>0 - ИД object). recivers - virainfoplugin, check off edit button and send EditObjectGeometry to other plugins.
    EditModeFinish,
    //! , параметр - bool (true - старт работы потока, false - окончание).
    BlockGUI,
    //!
    UpdateMark,

    //! from ViraInfoPlugin to ViraStatusBar
    //! параметр - QVariantList -> size==0 (abort) or size==3 : list[0] - type (defect=1, foto=2, foto360=3); list[1] - QPolygonF (possible size : 1(Point) or N(Area) ); list[2] - double (direction, for foto360 only !).
    MarkCreated,


};
}

#endif // BUSTAGS_H
