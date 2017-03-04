#ifndef BOOSTSIGNALADAPTERS_H
#define BOOSTSIGNALADAPTERS_H

#include <QObject>

namespace global_state
{

class BoostSignalUnaryReemiter32 : public QObject
{
    Q_OBJECT
public:
    BoostSignalUnaryReemiter32() {}
    void reemit(uint32_t entId);
signals:
    void signalBoostReemit(uint32_t entId);
};

class HandlerBoostSignalUnary32
{
    BoostSignalUnaryReemiter32* emiter;
public:
    HandlerBoostSignalUnary32(BoostSignalUnaryReemiter32* emiter);
    void operator() (uint32_t entId);
};

class BoostSignalUnaryReemiter64 : public QObject
{
    Q_OBJECT
public:
    BoostSignalUnaryReemiter64() {}
    void reemit(uint64_t entId);
signals:
    void signalBoostReemit(uint64_t entId);
};

class BoostSignalBinaryReemiter64 : public QObject
{
    Q_OBJECT
public:
    BoostSignalBinaryReemiter64() {}
    void reemit(uint64_t entId, bool on_off);
signals:
    void signalBoostReemit(uint64_t entId, bool on_off);
};

class HandlerBoostSignalUnary64
{
    BoostSignalUnaryReemiter64* emiter;
public:
    HandlerBoostSignalUnary64(BoostSignalUnaryReemiter64* emiter);
    void operator() (uint64_t entId);
};

class HandlerBoostSignalBinary64
{
    BoostSignalBinaryReemiter64* emiter;
public:
    HandlerBoostSignalBinary64(BoostSignalBinaryReemiter64* emiter);
    void operator() (uint64_t entId, bool on_off);
};

/*
    Пример использования :

    global_state::BoostSignalUnaryReemiter64 * _selectReemiter;
    _selectReemiter = new global_state::BoostSignalUnaryReemiter64;

    global_state::HandlerBoostSignalUnary64 * _selectHandler;
    _selectHandler = new global_state::HandlerBoostSignalUnary64(_selectReemiter);
    objrepr::RepresentationServer::instance()->objectManager()->objectSelected.connect(*_selectHandler);
    connect(_selectReemiter, SIGNAL(signalBoostReemit(uint64_t)), this, SLOT(slotSelectedObject(uint64_t)));
*/

}
#endif // BOOSTSIGNALADAPTERS_H
