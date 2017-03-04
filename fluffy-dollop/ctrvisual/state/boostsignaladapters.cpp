#include "boostsignaladapters.h"

void global_state::BoostSignalUnaryReemiter32::reemit(uint32_t entId)
{
    emit signalBoostReemit(entId);
}

global_state::HandlerBoostSignalUnary32::HandlerBoostSignalUnary32(BoostSignalUnaryReemiter32* _emiter)
{
    emiter = _emiter;
}

void global_state::HandlerBoostSignalUnary32::operator() (uint32_t entId)
{
    emiter->reemit(entId);
}

void global_state::BoostSignalUnaryReemiter64::reemit(uint64_t entId)
{
    emit signalBoostReemit(entId);
}

void global_state::BoostSignalBinaryReemiter64::reemit(uint64_t entId, bool on_off)
{
    emit signalBoostReemit(entId, on_off);
}

global_state::HandlerBoostSignalUnary64::HandlerBoostSignalUnary64(BoostSignalUnaryReemiter64* _emiter)
{
    emiter = _emiter;
}

void global_state::HandlerBoostSignalUnary64::operator() (uint64_t entId)
{
    emiter->reemit(entId);
}

global_state::HandlerBoostSignalBinary64::HandlerBoostSignalBinary64(BoostSignalBinaryReemiter64* _emiter)
{
    emiter = _emiter;
}

void global_state::HandlerBoostSignalBinary64::operator() (uint64_t entId, bool on_off)
{
    emiter->reemit(entId, on_off);
}

