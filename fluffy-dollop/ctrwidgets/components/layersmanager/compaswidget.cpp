#include "compaswidget.h"
#include <QPixmap>
#include <QPainter>
#include <QTransform>

CompasWidget::CompasWidget()
{
    setAngle(0);
}

void CompasWidget::setAngle(int angle)
{
    QPixmap pm1(":/img/compas.png");
    QPixmap pm2(pm1.size());
    pm2.fill(Qt::transparent);
    QPainter pr(&pm2);
    pr.setTransform(QTransform().translate(pm2.width()/2., pm2.height()/2.).rotate(angle).translate(-pm2.width()/2., -pm2.height()/2.));
    pr.setRenderHint(QPainter::SmoothPixmapTransform);
    pr.drawPixmap(0, 0, pm1);
    setPixmap(pm2);
}
