#ifndef ABSTRACTPROVIDERDEVICE_H
#define ABSTRACTPROVIDERDEVICE_H

#include <QObject>
#include <QUrl>

#include "abstractdataprovider.h"

class QIODevice;

namespace data_system
{
class AbstractProviderDevice : public QObject
{
Q_OBJECT

public:
    AbstractProviderDevice(QObject *parent = 0);

    /**
     * @brief Метод, предназначенный для получения поддерживаемых методов доступа к данным
     * @return список методов доспута
     */
    virtual QStringList methods() const = 0;

    /**
     * @brief Метод, предназанченный для полулчения указателя на устройство, походящее для метода
     * доступа, определяемого параметром method. Параметр method определяется QUrl::sceme()
     * После использования полученный объект должен быть удален получателем. Полученное устройство
     * должно быть готово к чтению/записи данных.
     * @param method значение метода доступа из URL
     * @return укзатель на QIODevice или 0 в случае, если подходящее утсроство не найдено
     */
    virtual QIODevice *getDevice(const QUrl &url, AbstractDataProvider::WorkMode mode = AbstractDataProvider::Both) = 0;

    /**
     * @brief Метод, предназначенный для получения объекта с мета-даными сформированными устройством
     * @return
     */
    virtual const MetaData &metaData() = 0;

    typedef AbstractProviderDevice *(*createFunction)();
};
}

#endif // ABSTRACTPROVIDERDEVICE_H
