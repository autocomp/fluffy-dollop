#include "googleadapter.h"
#include <QDebug>

GoogleAdapter::GoogleAdapter(const QUrl &url)
    : TmsBaseAdapter(url)
{
    setMaxZoomLevel(17);
}

QUrl GoogleAdapter::query(int x, int y, int z) const
{
//    int _x(x), _y(y), _z(z);

//    z += zLevelShift();
//    if(z == 0)
//        y = 0;
//    else
//        y = (pow(2,z + zLevelShiftForY())-1) - y;

    //qDebug() << "LocalTmsAdapter, IN : X :" << _x << ", Y :" << _y << ", Z :" << _z << ", OUT X :" << x << ", Y :" << y << ", Z :" << z;

    if(z > 17)
        z = 0;
    else
        z = 17-z;

    int a[3] = {z, x, y};
    QString queryString = serverPath();
    queryString.replace(order[2][0],2, loc.toString(a[order[2][1]]));
    queryString.replace(order[1][0],2, loc.toString(a[order[1][1]]));
    queryString.replace(order[0][0],2, loc.toString(a[order[0][1]]));

    QUrl url;
    url.setScheme(scheme());
    url.setHost(serverHost());
    url.setPort(serverPort());
    url.setPath(queryString);

    return url;
}
