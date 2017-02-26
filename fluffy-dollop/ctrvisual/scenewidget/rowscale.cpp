#include "rowscale.h"
//#include "mapviewer.h"
//#include "coordconvert.h"
#include "QDebug"
#include <QPainter>
//#include "viewercontroller.h"
#include <QVector>
#include <QVBoxLayout>


RowScale::RowScale(QWidget *parent) :
    QWidget(parent)
{
    m_distanceMeters = 0;
    m_showDistValue = false;
    m_distStr = QString("");
    m_mInPixel = 0;
    m_userSetMeters = 0;

    //m_fixedPixelLen = 0;
    qreal dim[] = { 0.01 , 0.05, /*см*/
                    0.1, 0.5, /*дм*/
                    1.0, 5.0, 10.0, 20.0, 50.0, 100.0, 200.0, 500.0, /*метры*/
                    1000.0, 2000.0, 2500.0, 5000.0, 10000.0, 25000.0, 50000.0, 100000.0, /*км*/
                    200000.0, 500000.0, 1000000.0, 2000000.0, 2500000.0, 5000000. /*км*/ };

    int elemCount = ( sizeof(dim)/sizeof(qreal));
    for(int i=0; i< elemCount; i++  )
        m_defaultDim.append(dim[i]);


    pm = QPixmap("qrc:/rowScaleBack1.png");
    setFixedSize(160,45);
    m_scaleWidth = 120;
    m_settingsPanelSize = size();

    m_pRowScaleInner = new RowScaleInner(0);
    m_pRowScaleInner->setObjectName("RowScale_Background");

    QWidget * wdg = new QWidget(this);
    wdg->setObjectName("RowScale_Background");


    QHBoxLayout * lt1 = new QHBoxLayout(wdg);
    lt1->setMargin(0);
    lt1->setSpacing(0);
    lt1->addWidget(m_pRowScaleInner,1);

    QHBoxLayout * lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(wdg,1);


    m_pTextUpLabel = new QLabel(this);
    m_pTextDownLabel = new QLabel(this);

    m_pTextUpLabel->setObjectName("RowScale_TextUpLabel");
    m_pTextDownLabel->setObjectName("RowScale_TextDownLabel");

    m_pTextUpLabel->setFixedSize(160, 14);
    m_pTextDownLabel->setFixedSize(160, 14);

    m_pTextUpLabel->setAlignment(Qt::AlignCenter);
    m_pTextDownLabel->setAlignment(Qt::AlignCenter);
    m_pTextUpLabel->move(0, 0);
    m_pTextDownLabel->move(0, 31);

    m_pTextUpLabel->setText("up");
    m_pTextDownLabel->setText("down");
}




void RowScale::switchToMeasureDistance()
{
    m_showDistValue = true;
    m_pRowScaleInner->switchToMeasureDistance();
    update();
}

void RowScale::switchToScale()
{
    m_showDistValue = false;
    m_pRowScaleInner->switchToScale();
    update();
}

void RowScale::setDistanceValue(double distanceMeters)
{
    QString tmp;
    m_distanceMeters = distanceMeters;
    m_distStr = convertMetersToNormString(m_distanceMeters);
    update();
}


void RowScale::setInterpixelDistance(qreal meters)
{
    if(m_mInPixel != meters)
    {
        m_userSetMeters  = 0;
    }

    m_mInPixel = meters;
    calcRowScale();
    update();
}


void RowScale::setUserSet(qreal meters)
{
    m_userSetMeters = meters;

    //m_fixedPixelLen = 0;
    calcRowScale();
    //m_fixedPixelLen = m_upScalePixelLen;
    update();

}


RowScale::~RowScale()
{


}


void RowScale:: mousePressEvent(QMouseEvent *e)
{
    emit signalShowScaleControl();
}

int RowScale::getScalePxLenght()
{
    return width();
}


QString RowScale::convertMetersToNormString(qreal meters)
{
    QString normStr;
    /*переводим в адекватные единицы измерения*/
    if(meters < 0.1)
    {
        normStr = normStr.setNum( qRound(meters*100),10 ) + QString(" ") + QString(tr("cm"));
    }


    if(meters < 1)
    {
        normStr = normStr.setNum( qRound(meters*10),10 ) + QString(" ")+ QString(tr("dm"));

    }

    if(meters >= 1)
    {
        normStr = normStr.setNum( qRound(meters), 10 ) + QString(" ")+ QString(tr("m"));

    }

    if(meters>=1000 )
    {
        normStr = normStr.setNum( qRound(meters/1000.0), 10 ) + QString(" ")+ QString(tr("km"));
    }

    return normStr;
}



void RowScale::calcRowScale()
{
    double mInPixel = m_mInPixel;
    if(0 == mInPixel)
        return;

    double pxInM = 1.0/mInPixel;

    /*определяем длину линейки и текст: линейка должна быть не более maxScaleLenght и не менее
       minScaleLenght px и такой, чтобы величина была кратна 10 или 20 или 50 или100... */

    qreal scaleMaxLenPx = width() - 10;

    //qreal scaleDownUnnormLenM = scaleMaxLenPx * m_mInPixel;
    /*ищем ближайшее нормированное значение, которое будет меньше заданного и
     *максимально приближенно к scaleDownUnnormLenM */

    qreal scaleDownNormLenM = scaleMaxLenPx * m_mInPixel; //m_defaultDim[0];
    /*foreach (qreal curDim, m_defaultDim)
    {
        if(curDim > scaleDownUnnormLenM)
            break;
        else
            scaleDownNormLenM = curDim;
    }*/

    m_outValueDown = scaleDownNormLenM;
    m_outTextDown = convertMetersToNormString(scaleDownNormLenM);
    m_downScalePixelLen = scaleMaxLenPx ;//(int)(scaleDownNormLenM*(1/m_mInPixel));

    /*теперь верхнее значение*/



    if(0 != m_userSetMeters)
    {
        /*пользователь захотел зум*/
        /*foreach (qreal curDim, m_defaultDim)
        {
            if(curDim > scaleMaxLenPx)
                break;
            else
                scaleDownNormLenM = curDim;
        }*/

        m_upScalePixelLen = m_userSetMeters*(1.0/m_mInPixel) ;
        m_outValueUp = m_userSetMeters;
        m_outTextUp = convertMetersToNormString(m_userSetMeters);
    }
    else
    {
        /*изменение масштаба от колеса мыши*/
        foreach (qreal curDim, m_defaultDim)
        {
            if(curDim*(1.0/m_mInPixel) > scaleMaxLenPx)
                break;
            else
                scaleDownNormLenM = curDim;
        }
        m_upScalePixelLen = scaleDownNormLenM*(1.0/m_mInPixel);
        m_outValueUp = (m_upScalePixelLen*m_mInPixel) ;
        m_outTextUp = convertMetersToNormString(m_outValueUp);
    }

    m_pTextUpLabel->setText(m_outTextUp);
    m_pTextDownLabel->setText(m_outTextDown);

    m_pRowScaleInner->setScaleLen(m_upScalePixelLen, m_downScalePixelLen);

}

