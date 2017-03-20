#ifndef TEMPDATATYPIES_H
#define TEMPDATATYPIES_H

#include <QString>
#include <QSet>

namespace temp_data
{

enum class RoomState
{
    Free = 0,
    InTenant = 1,
    Reconstruction = 2,
    Sale = 3
};

struct RoomInfo
{
    double square = 0;
    RoomState state = RoomState::Free;
    qulonglong tenant = 0;
};

struct AreaInfo
{
    double squareFree = 0;
    double squareInTenant = 0;
    QSet<qulonglong> tenants;
    uint debt = 0;
};

struct Tenant
{
    QString name;
    uint debt = 0;
};

}

#endif // TEMPDATATYPIES_H
