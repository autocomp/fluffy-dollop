#ifndef GOOGLEADAPTER_H
#define GOOGLEADAPTER_H
#include "tmsbaseadapter.h"

#include <QObject>
#include <QLocale>
#include <QUrl>

class GoogleAdapter : public TmsBaseAdapter
{
public:
    GoogleAdapter(const QUrl& url);
    virtual ~GoogleAdapter() {}
    virtual QUrl query(int x, int y, int z) const;
};

#endif // GOOGLEADAPTER_H
