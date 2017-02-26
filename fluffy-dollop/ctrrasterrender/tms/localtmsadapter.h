#ifndef LOCALTMSADAPTER_H
#define LOCALTMSADAPTER_H
#include "tmsbaseadapter.h"

#include <QObject>
#include <QLocale>
#include <QUrl>

class LocalTmsAdapter : public TmsBaseAdapter
{
public:
    LocalTmsAdapter(const QUrl& url);
    virtual ~LocalTmsAdapter() {}
    virtual QUrl query(int x, int y, int z) const;
};


#endif









