#ifndef SINCXMLLOADER_H
#define SINCXMLLOADER_H

#include <QUrl>

class SincXmlLoader
{
public:
    static bool loadXml(const QUrl& url, QByteArray& xml, int defaultPort = 80, uint msecTimeout = 3000);
};

#endif
