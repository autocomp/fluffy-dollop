#include "coordbar.h"
#include "QPainter"
#include <QHBoxLayout>
#include <QStyle>
#include <QFile>
#include <QAbstractItemView>
//#include <QPlastiqueStyle>
//#include <ctrvisual/providerdelegate/dpfcoordtransformer.h>
#include <stdio.h>

CoordBar::CoordBar(QWidget *parent) :
    QWidget(parent),
    m_visible(false)
{
    setFixedHeight(30);
    setFixedWidth(670);//595);
//    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QWidget * wdg = new QWidget(this);
    wdg->setObjectName("CoordBar_Background");

    m_curLattitude = 0;
    m_curLongtitude = 0;
    m_curHeight = 0;

    m_comboBox = new QComboBox(wdg);
    m_comboBox->setFrame(false);
    m_comboBox->setCurrentIndex(0);
    m_comboBox->setFixedHeight(20);
//    m_comboBox->setFixedWidth(170);
    m_comboBox->setObjectName("CoordBar_CoordSystemComboBox");

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentSystemChangedLocal(int)));

    m_pLongitudeLabel = new QLabel(wdg);
    m_pLatitudeLabel = new QLabel(wdg);
    m_pHeightLabel = new QLabel(wdg);

    m_pLongitudeLabel->setFixedWidth(140);//120
    m_pLatitudeLabel->setFixedWidth(140);
    m_pHeightLabel->setFixedWidth(140);

    m_pLongitudeLabel->setObjectName("CoordBar_LongitudeLabel");
    m_pLatitudeLabel->setObjectName("CoordBar_LatitudeLabel");
    m_pHeightLabel->setObjectName("CoordBar_ HeightLabel");

    QHBoxLayout * lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(wdg);


    QHBoxLayout * mainLayout = new QHBoxLayout(wdg);
    mainLayout->setMargin(3);
    mainLayout->setSpacing(3);
    mainLayout->addWidget(m_comboBox/*, 0, Qt::AlignCenter | Qt::AlignLeft*/);
    mainLayout->addWidget(m_pLongitudeLabel);
    mainLayout->addWidget(m_pLatitudeLabel);
    mainLayout->addWidget(m_pHeightLabel);
//    mainLayout->addStretch(10);

    m_objectSelected = true;

    m_longStr = QString("");
    m_latStr = QString("");
    m_heightStr = QString("");

    m_geoCoord = true;

    setHeightValue(0);
}

void CoordBar::clearCoordSystem()
{
    if (m_comboBox)
        m_comboBox->clear();
}

void CoordBar::addCoordSystem(QString coordSystemName, int coordSystemId)
{
    int curIndex = m_comboBox->currentIndex();

    m_coordSystems.insert(coordSystemId, coordSystemName);
    m_comboBox->addItem(coordSystemName);
    if(curIndex < 0)
        m_comboBox->setCurrentIndex(0);
    else
        m_comboBox->setCurrentIndex(curIndex);
}

void CoordBar::setHeightValue(double meters)
{
    double dist = 0;

    if (meters < 1000)
    {
        m_heightStr = m_heightStr.setNum(meters, 'f',1) + QString(tr("m"));
    } else if (meters < 1000000)
    {
        dist = meters / 1000.0;
        m_heightStr = m_heightStr.setNum(dist, 'f',2) + QString(tr("km"));
    } else
    {
        dist = meters / 1000.0;
        m_heightStr = m_heightStr.setNum((int)dist,10) + QString(tr("km"));
    }

    m_pHeightLabel->setText(m_heightStr);
}

void CoordBar::initPixelCoordSystems()
{
    addCoordSystem(QString::fromUtf8("Пиксель"), 0);
}

void CoordBar::initGeoCoordSystems()
{
    if( m_geoCoord )
    {
        addCoordSystem(QString::fromUtf8("WGS-84, 00.000000°"), 1);
        addCoordSystem(QString::fromUtf8("WGS-84, 00°00.000\'"), 2);
        addCoordSystem(QString::fromUtf8("WGS-84, 00°00'00.0\""), 3);
        addCoordSystem(QString::fromUtf8("СК-42, 00.000000°"), 4);
        addCoordSystem(QString::fromUtf8("СК-42, 00°00.000\'"), 5);
        addCoordSystem(QString::fromUtf8("СК-42, 00°00'00.0\""), 6);
        addCoordSystem(QString::fromUtf8("СК-42(ГК), метры"), 7);
        addCoordSystem(QString::fromUtf8("СК-95, 00.000000°"), 8);
        addCoordSystem(QString::fromUtf8("СК-95, 00°00.000\'"), 9);
        addCoordSystem(QString::fromUtf8("СК-95, 00°00'00.0\""), 10);
        addCoordSystem(QString::fromUtf8("ПЗ-90.11, 00.000000°"), 11);
        addCoordSystem(QString::fromUtf8("ПЗ-90.11, 00°00.000\'"), 12);
        addCoordSystem(QString::fromUtf8("ПЗ-90.11, 00°00'00.0\""), 13);
        addCoordSystem(QString::fromUtf8("N UPS, метры"), 14);
        setCurCoordSystem(1);
    }
    else
    {
        addCoordSystem(QString::fromUtf8("CCK, 00.000000°"), 15);
        addCoordSystem(QString::fromUtf8("CCK, 00°00.000\'"), 16);
        addCoordSystem(QString::fromUtf8("CCK, 00°00'00.0\""), 17);
        setCurCoordSystem(15);
    }
}

void CoordBar::initCCKCoordSystems()
{
    addCoordSystem(QString::fromUtf8("CCK, 00.000000°"), 15);
    addCoordSystem(QString::fromUtf8("CCK, 00°00.000\'"), 16);
    addCoordSystem(QString::fromUtf8("CCK, 00°00'00.0\""), 17);
}

void CoordBar::convertionFromWGS84(double longitude, double latitude, double height)
{
//    dpf::geo::GeoPoint geo_src( latitude, longitude, height);
//    dpf::geo::GeoPoint geo_dst;


    char * h = new char[50];
    sprintf(h,"%d ", static_cast<int>(height));

//    systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());

    int currentSystem = getCurCoordSystem();

    QString lng, lat, ht;

    if((currentSystem == 7) || (currentSystem == 14))
    {
//        lng = QString(tr("Y: ")) + coordToString(geo_dst.longitude()) + QString((" ")) + QString(tr("m"));
//        lat = QString(tr("X: ")) + coordToString(geo_dst.latitude()) + QString((" ")) + QString(tr("m"));
//        ht = QString(tr("H: ")) + QString(h) + QString(tr("m"));
    }
    else
    {
//        lng = QString(tr("Long: ")) + coordToString(geo_dst.longitude());
//        lat = QString(tr("Lat: ")) + coordToString(geo_dst.latitude());
//        ht = QString(tr("H: ")) + QString(h) + QString(tr("m"));
    }
    setCurrentCoords(lng, lat, ht);

    //    setCurrentCoords(coordToString(geo_dst.longitude()), coordToString(geo_dst.latitude()), QString(h));

    delete [] h;
    return;
}

void CoordBar::setCCKCoord(double longitude, double latitude, double height)
{
    char * h = new char[50];
    sprintf(h,"%d ", static_cast<int>(height));

    QString lng = QString(tr("Long: ")) + coordToString(longitude);
    QString lat = QString(tr("Lat: ")) + coordToString(latitude);
    QString ht = QString(tr("H: ")) + QString(h) + QString(tr("m"));
    setCurrentCoords(lng, lat, ht);

    delete [] h;
    return;
}

void CoordBar::convertionFromWGS84(double longitude, double latitude)
{
//    dpf::geo::GeoPoint geo_src( latitude, longitude, 0);
//    dpf::geo::GeoPoint geo_dst;

//    systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());

    QString lng, lat;
    int currentSystem = 0; //getCoordinateType();

    if((currentSystem == 7) || (currentSystem == 14))
    {
//        lng = QString(tr("Y: ")) + coordToString(geo_dst.longitude()) + QString((" ")) + QString(tr("m"));;
//        lat = QString(tr("X: ")) + coordToString(geo_dst.latitude()) + QString((" ")) + QString(tr("m"));;
    }
    else
    {
//        lng = QString(tr("Long: ")) + coordToString(geo_dst.longitude());
//        lat = QString(tr("Lat: ")) + coordToString(geo_dst.latitude());
    }

    //    QString lng = QString(tr("Long: ")) + coordToString(geo_dst.longitude());
    //    QString lat = QString(tr("Lat: ")) + coordToString(geo_dst.latitude());
    setCurrentCoords(lng, lat, QString(""));

    //setCurrentCoords(coordToString(geo_dst.longitude()), coordToString(geo_dst.latitude()), QString(""));

    return;
}

//dpf::geo::CoordinateSystem CoordBar::getCoordinateType()
//{
//    int currentSystem = getCurCoordSystem();
//    switch(currentSystem)
//    {
//    case 4 :
//    case 5 :
//    case 6 :
//        return dpf::geo::CoordinateSystem::PULKOVO_1942;
//    case 7 :
//        return dpf::geo::CoordinateSystem::GAUSS_KRUGER;
//    case 8 :
//    case 9 :
//    case 10 :
//        return dpf::geo::CoordinateSystem::SK_95;
//    case 11 :
//    case 12 :
//    case 13 :
//        return dpf::geo::CoordinateSystem::PZ_9011;
//    case 14 :
//        return dpf::geo::CoordinateSystem::NORTH_POLAR;
//    }
//    return dpf::geo::CoordinateSystem::WGS_84;
//}

QString CoordBar::coordToString(double data)
{
    QString ret;
    int currentSystem = getCurCoordSystem();
    switch(currentSystem)
    {
    case 1 :
    case 4 :
    case 8 :
    case 11:
    case 15:
//        ret = QString::fromStdString(angleConvertion.dec_deg(data));
        break;
    case 2 :
    case 5 :
    case 9 :
    case 12 :
    case 16 :
//        ret = QString::fromStdString(angleConvertion.deg_min(data));
        break;
    case 3 :
    case 6 :
    case 10 :
    case 13 :
    case 17 :
//        ret = QString::fromStdString(angleConvertion.deg_min_sec(data));
        break;
    case 7 :
    case 14 :
        ret = QString::number(data, 'f', 0);
        break;
           }
    return ret;
}

void CoordBar::setObjectSelected(bool objSelected )
{
    m_objectSelected = objSelected;
}


int CoordBar::getCurCoordSystem()
{
    return m_coordSystems.key(m_comboBox->currentText());
}

void CoordBar::setCurCoordSystem(int coordSystemId)
{
    if(m_coordSystems.contains(coordSystemId))
    {
        int id = m_comboBox->findText(m_coordSystems[coordSystemId]);
        m_comboBox->setCurrentIndex(id);
        //        currentCoordSystem = id;
        emit currentCoordSystemChanged(coordSystemId);
    }
}


void CoordBar::slotCurrentSystemChangedLocal(int newSystem)
{
    int curCoordSystem = m_coordSystems.key(m_comboBox->currentText());

    setCurrentCoords("","","");
    emit currentCoordSystemChanged(curCoordSystem);

}


void CoordBar::setCurrentCoords(QString lattitude, QString longtitude, QString heightMeters)
{
    m_longStr = longtitude;
    m_latStr = lattitude;
    m_heightStr = heightMeters;

    m_pLongitudeLabel->setText(m_longStr);
    m_pLatitudeLabel->setText(m_latStr);
    m_pHeightLabel->setText(m_heightStr);
}

void CoordBar::setCurrentCoords(qreal lattitude, qreal longtitude, int lattLongPrecision, double heightMeters)
{
    m_curLattitude = lattitude;
    m_curLongtitude = longtitude;

    setHeightValue(heightMeters);

    m_longStr = m_longStr.setNum(m_curLattitude,'f', lattLongPrecision);
    m_latStr = m_latStr.setNum(m_curLongtitude,'f' ,lattLongPrecision);

    m_pLongitudeLabel->setText(m_longStr);
    m_pLatitudeLabel->setText(m_latStr);
}

CoordBar::~CoordBar()
{

}

void CoordBar::setHeightLabelShown(bool isShown)
{
    m_pHeightLabel->setVisible(isShown);
}

bool CoordBar::getGeoCoord()
{
    return m_geoCoord;
}

void CoordBar::setGeoCoord( bool coord )
{
    m_geoCoord = coord;
}
