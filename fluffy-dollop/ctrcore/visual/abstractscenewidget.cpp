#include "abstractscenewidget.h"

using namespace sw;

AbstractSceneWidget::AbstractSceneWidget(QWidget *pParent):
    QWidget(pParent)
{

}

void AbstractSceneWidget::resizeEvent(QResizeEvent *e)
{
    emit newSize(e->size().width(), e->size().height());
    QWidget::resizeEvent(e);
}
