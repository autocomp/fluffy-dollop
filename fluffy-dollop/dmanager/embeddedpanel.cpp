#include "embeddedapp.h"
#include "embeddedpanel.h"
#include "embeddedwidget.h"
#include <QDebug>
#include <QPainter>
#include <QStyleOption>

using namespace ew;

/// FIXME: если воткнули виджет и наша панель стала по размерам больше родителя
/// , то уменьшаем размеры виджетов до минимума и только после этого останавливаемся
EmbeddedPanel::EmbeddedPanel(QWidget *parent) : QWidget(parent)
{
    m_minimumChildHeight = 1e8;
    m_minimumChildWidth = 1e8;
    m_personalPanel = false;
    m_pHighlightedEW = nullptr;
    m_highlightSpace = false;
}


void EmbeddedPanel::paintEvent(QPaintEvent *)
{
    QStyleOption opt;

    opt.init(this);
    QPainter pr(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &pr, this);

    if(m_highlightSpace && m_highlightRect.isValid())
    {
        pr.setPen(QColor(0, 50, 200, 128));
        pr.setBrush(QBrush(QColor(0, 50, 200, 128)));
        pr.drawRect(m_highlightRect);
    }
}


void EmbeddedPanel::fixIntersectedWidgets(EmbeddedWidgetBaseStructPrivate          *ewStructPriv,
                                          QQueue<EmbeddedWidgetBaseStructPrivate *> intersectedWidgets,
                                          InsertPolicy                              insertPolicy)
{
    while(!intersectedWidgets.empty())
    {
        EmbeddedWidgetBaseStructPrivate *item = intersectedWidgets.front();
        intersectedWidgets.pop_front();
        QPoint insertPos = item->embWidgetPtr->pos();

        if(InsertPolicy::Auto == insertPolicy)
        {
            // Выбираем политику, при которой достигается наименьший сдвиг
            int shiftX = qAbs(insertPos.x() - (ewStructPriv->embWidgetPtr->x() + ewStructPriv->embWidgetPtr->width()));
            int shiftY = qAbs(insertPos.y() - (ewStructPriv->embWidgetPtr->y() + ewStructPriv->embWidgetPtr->height()));
            insertPolicy = shiftX < shiftY ? InsertPolicy::Horizontal : InsertPolicy::Vertical;
        }

        if(InsertPolicy::Horizontal == insertPolicy)
        {
            insertPos.setX(ewStructPriv->embWidgetPtr->x() + ewStructPriv->embWidgetPtr->width());
        }
        else // (InsertPolicy::Vertical == insertPolicy)
        {
            insertPos.setY(ewStructPriv->embWidgetPtr->y() + ewStructPriv->embWidgetPtr->height());
        }

        ewApp()->changeWidgetGeometry(item->embWidgetPtr->id(), insertPos, item->embWidgetPtr->size());
        fixIntersectedWidgets(item, getIntersectedWidgets(item), insertPolicy);
    }
}


QQueue<EmbeddedWidgetBaseStructPrivate *> EmbeddedPanel::getIntersectedWidgets(EmbeddedWidgetBaseStructPrivate *widget)
{
    QQueue<EmbeddedWidgetBaseStructPrivate *> queue;

    foreach (EmbeddedWidgetBaseStructPrivate * child, m_widgetList)
    {
        if(widget == child)
        {
            continue;
        }

        QRect g1 = widget->embWidgetPtr->geometry();
        QRect g2 = child->embWidgetPtr->geometry();

        if(g1.intersects(g2))
        {
            queue.push_back(child);
        }
    }

    return queue;
}


QPoint EmbeddedPanel::getOptimalPosition(QPoint localPos) const
{
    bool search = true;
    int x = localPos.x();
    int y = localPos.y();

    // X
    while(search)
    {
        x -= m_minimumChildWidth;

        if(x < 0)
        {
            x = 0;
            search = false;
        }

        EmbeddedWidgetBaseStructPrivate *child = childAtNotPreview(x, y);

        if(nullptr != child)
        {
            x = child->embWidgetPtr->geometry().right() + 1;
            search = false;
        }
    }

    // Y
    search = true;

    while(search)
    {
        y -= m_minimumChildHeight;

        if(y < 0)
        {
            y = 0;
            search = false;
        }

        EmbeddedWidgetBaseStructPrivate *child = childAtNotPreview(x, y);

        if(nullptr != child)
        {
            y = child->embWidgetPtr->geometry().bottom() + 1;
            search = false;
        }
    }

    return QPoint(x, y);
}


QSize EmbeddedPanel::getOptimalSize(EmbeddedWidgetBaseStructPrivate *ewStructPriv)
{
    if(nullptr == ewStructPriv)
    {
        return QSize(-1, -1);
    }

    bool search = true;
//    int x = ewStructPriv->embWidgetPtr->geometry().right() + 1;
//    int y = ewStructPriv->embWidgetPtr->geometry().bottom() + 1;
    int x = ewStructPriv->embWidgetPtr->x();
    int y = ewStructPriv->embWidgetPtr->y();

    // X
    while(search)
    {
        x += m_minimumChildWidth;

        if(x > width())
        {
            x = width();
            search = false;
        }

        EmbeddedWidgetBaseStructPrivate *childB = childAtNotPreview(x, y);
        EmbeddedWidgetBaseStructPrivate *childT = childAtNotPreview(x, ewStructPriv->embWidgetPtr->geometry().bottom());

        if(nullptr != childB && childB != ewStructPriv)
        {
            x = childB->embWidgetPtr->geometry().left() - 1;
            search = false;
        }

        if(nullptr != childT && childT != ewStructPriv)
        {
            x = childT->embWidgetPtr->geometry().left() - 1;
            search = false;
        }
    }

    // Y
    search = true;

    while(search)
    {
        y += m_minimumChildHeight;

        if(y > height())
        {
            y = height();
            search = false;
        }

        EmbeddedWidgetBaseStructPrivate *childL = childAtNotPreview(ewStructPriv->embWidgetPtr->x(), y);
        EmbeddedWidgetBaseStructPrivate *childR = childAtNotPreview(x, y);

        if(nullptr != childR && childR != ewStructPriv)
        {
            y = childR->embWidgetPtr->geometry().top() - 1;
            search = false;
        }

        if(nullptr != childL && childL != ewStructPriv)
        {
            y = childL->embWidgetPtr->geometry().top() - 1;
            search = false;
        }
    }

    return QSize(x - ewStructPriv->embWidgetPtr->x(), y - ewStructPriv->embWidgetPtr->y());
}


EmbeddedWidgetBaseStructPrivate *EmbeddedPanel::childAtNotPreview(int x, int y) const
{
    return childAtNotPreview(QPoint(x, y));
}


EmbeddedWidgetBaseStructPrivate *EmbeddedPanel::childAtNotPreview(QPoint pos) const
{
    QWidget *child = childAt(pos);
    EmbeddedWidgetBaseStructPrivate *answer = nullptr;
    QWidget *childParent = child;

    while(0 != childParent)
    {
        if(dynamic_cast<EmbeddedWidget *>(childParent))
        {
            break;
        }

        childParent = childParent->parentWidget();
    }

    foreach (auto st, m_widgetList)
    {
        if(!st->embWidgetStruct->addHided && st->embWidgetPtr == childParent)
        {
            answer = st;
            break;
        }
    }

    return answer;
}


void EmbeddedPanel::insertEmbeddedWidget(ew::EmbeddedWidgetBaseStructPrivate *ewStructPriv, QPoint localPos)
{
    highlightWidgetTurnOff();
    InsertPolicy insertPolicy = InsertPolicy::Auto;
    QPoint insertPos = localPos;
    EmbeddedWidgetBaseStructPrivate *intersectedWidget = childAtNotPreview(insertPos);

    if(nullptr == intersectedWidget)
    {
        insertPos = getOptimalPosition(insertPos);
    }
    else
    {
        QPoint ewPos(insertPos.x() - intersectedWidget->embWidgetPtr->x(),
                     insertPos.y() - intersectedWidget->embWidgetPtr->y());
        HighlightSection hs = intersectedWidget->embWidgetPtr->getCardinalDirection(ewPos);
        const QRect & geom = intersectedWidget->embWidgetPtr->geometry();

        if(HS_BOTTOM == hs)
        {
            insertPolicy = InsertPolicy::Vertical;
            insertPos = QPoint(geom.left(), geom.bottom() + 1);
        }
        else if(HS_TOP == hs)
        {
            insertPolicy = InsertPolicy::Vertical;
            insertPos = QPoint(geom.left(), geom.top());
        }
        else if(HS_LEFT == hs)
        {
            insertPolicy = InsertPolicy::Horizontal;
            insertPos = QPoint(geom.left(), geom.top());
        }
        else if(HS_RIGHT == hs)
        {
            insertPolicy = InsertPolicy::Horizontal;
            insertPos = QPoint(geom.right() + 1, geom.top());
        }
    }

    ewStructPriv->embWidgetPtr->setParent(this);
    ewApp()->changeWidgetGeometry(ewStructPriv->embWidgetPtr->id(), insertPos, ewStructPriv->embWidgetStruct->size);
    ewStructPriv->embWidgetPtr->setVisible(true);
    m_widgetList.append(ewStructPriv);

    fixIntersectedWidgets(ewStructPriv, getIntersectedWidgets(ewStructPriv), insertPolicy);
    m_minimumChildHeight = qMin(m_minimumChildHeight, ewStructPriv->embWidgetStruct->size.height());
    m_minimumChildWidth = qMin(m_minimumChildWidth, ewStructPriv->embWidgetStruct->size.width());

    updateGeometry();
}

void EmbeddedPanel::insertEmbeddedWidgetForce(EmbeddedWidgetBaseStructPrivate *ewStructPriv, QPoint localPanelPos)
{
    ewStructPriv->embWidgetPtr->setParent(this);
    ewApp()->changeWidgetGeometry(ewStructPriv->embWidgetPtr->id(), localPanelPos, ewStructPriv->embWidgetStruct->size);
    ewStructPriv->embWidgetPtr->setVisible(true);
    m_widgetList.append(ewStructPriv);

//    fixIntersectedWidgets(ewStructPriv, getIntersectedWidgets(ewStructPriv), insertPolicy);
    m_minimumChildHeight = qMin(m_minimumChildHeight, ewStructPriv->embWidgetStruct->size.height());
    m_minimumChildWidth = qMin(m_minimumChildWidth, ewStructPriv->embWidgetStruct->size.width());

    updateGeometry();
}


void EmbeddedPanel::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
}


void EmbeddedPanel::removeEmbeddedWidget(ew::EmbeddedWidgetBaseStructPrivate *embStrPriv)
{
    m_widgetList.removeAll(embStrPriv);
    QPoint p = mapToGlobal(embStrPriv->embWidgetStruct->alignPoint.toPoint());
    ewApp()->changeWidgetGeometry(embStrPriv->embWidgetPtr->id(), p, embStrPriv->embWidgetPtr->size());

    embStrPriv->embWidgetPtr->setParent(nullptr);

    embStrPriv->embWidgetPtr->setVisible(true);
}


void EmbeddedPanel::clear()
{
    foreach (ew::EmbeddedWidgetBaseStructPrivate * embStrPriv, m_widgetList)
    {
        removeEmbeddedWidget(embStrPriv);
    }
}


void EmbeddedPanel::highlightWidget(QPoint localPos, QSize sz)
{
    highlightWidgetTurnOff();

    m_pHighlightedEW = childAtNotPreview(localPos);

    if(nullptr == m_pHighlightedEW)
    {
        m_highlightSpace = true;
        m_highlightRect = QRect(getOptimalPosition(localPos), sz);
        update();
        return;
    }

    m_highlightSpace = false;
    update();

    QPoint ewPos(
        localPos.x() - m_pHighlightedEW->embWidgetPtr->x(),
        localPos.y() - m_pHighlightedEW->embWidgetPtr->y()
        );

    m_pHighlightedEW->embWidgetPtr->setHighlightedSection(
        m_pHighlightedEW->embWidgetPtr->getCardinalDirection(ewPos));
    m_pHighlightedEW->embWidgetPtr->update();
}


void EmbeddedPanel::highlightWidgetTurnOff()
{
    m_highlightSpace = false;

    if(nullptr != m_pHighlightedEW)
    {
        m_pHighlightedEW->embWidgetPtr->setHighlightedSection(HS_NO_HIGHLIGHT);
        m_pHighlightedEW->embWidgetPtr->update();
        m_pHighlightedEW = nullptr;
    }

    update();
}


bool EmbeddedPanel::canAdd(EmbeddedWidgetType type)
{
    if(m_widgetList.empty())
    {
        return true;
    }

    return type == m_widgetList.first()->type();
}


void EmbeddedPanel::optimizeSpace()
{
    if (m_widgetList.isEmpty())
    {
        return;
    }

    const int MAX_WIDTH = width();
    QVector<int> levels;
    QVector<int> floorX;
    QVector<int> ceilX;

    QVector<QVector<int> > shiftsMap;
    QVector<QVector<int> > heightsMap;

    qSort(m_widgetList.begin(), m_widgetList.end(), [] (const ew::EmbeddedWidgetBaseStructPrivate *lhs
                                                        , const ew::EmbeddedWidgetBaseStructPrivate *rhs) -> bool
    {
        if(lhs->embWidgetPtr->height() == rhs->embWidgetPtr->height())
        {
            return lhs->id < rhs->id;
        }

        return lhs->embWidgetPtr->height() > rhs->embWidgetPtr->height();
    });

    for(int i = 0; i < m_widgetList.size(); ++i)
    {
        // Подготовка
        QWidget *widget = m_widgetList[i]->embWidgetPtr;
        bool found = false;
        QPoint pos(0, 0);

        // Поиск положения на уровнях
        for(int j = 0; j < levels.size(); ++j)
        {
            auto & heights = heightsMap[j];
            auto & shifts = shiftsMap[j];

            // Поиск на "потолке"
            if(ceilX[j] + widget->width() <= MAX_WIDTH)
            {
                pos.setX(ceilX[j]);
                ceilX[j] += widget->width();
                heights.last() = widget->height();
                shifts.last() = pos.x();
                heights.push_back(0);
                shifts.push_back(MAX_WIDTH);
                found = true;
                break;
            }

            // Поиск на "полу"
            auto it = std::upper_bound(shifts.begin(), shifts.end(), floorX[j] - widget->width());
            int index = static_cast<int>(it - shifts.begin());

            if(index > 0 && floorX[j] - widget->width() != shifts[index])
            {
                index--;
            }

            if(floorX[j] - widget->width() >= 0
               && heights[index] + widget->height() <= levels[j])
            {
                pos.setX(floorX[j] - widget->width());
                pos.setY(pos.y() + levels[j] - widget->height());
                floorX[j] -= widget->width();
                found = true;
                break;
            }

            // Переход к следующему шагу
            pos.setX(0);
            pos.setY(pos.y() + levels[j]);
        }

        if(!found)
        {
            levels.append(widget->height());
            ceilX.append(widget->width());
            floorX.append(MAX_WIDTH);

            QVector<int> heights;
            heights.push_back(widget->height());
            heights.push_back(0);
            heightsMap.push_back(heights);

            QVector<int> shifts;
            shifts.push_back(widget->width());
            shifts.push_back(MAX_WIDTH);
            shiftsMap.push_back(shifts);
        }

        // Применение изменений
        ewApp()->changeWidgetGeometry(m_widgetList[i]->embWidgetPtr->id(), pos, m_widgetList[i]->embWidgetPtr->size());
    }
}


void EmbeddedPanel::resizeEvent(QResizeEvent *)
{
    if(m_personalPanel)
    {
        optimizeWidgetSizes();
    }
}


void EmbeddedPanel::setWidget(ew::EmbeddedWidgetBaseStructPrivate *embStrPriv)
{
    if(!m_widgetList.isEmpty())
    {
        qWarning() << __FUNCTION__ << "Unexpected behaviour";
        return;
    }

    m_personalPanel = true;

    insertEmbeddedWidget(embStrPriv, QPoint(0, 0));
    optimizeWidgetSizes();
}

QRect EmbeddedPanel::getContentRect()
{
    QRectF res;
    foreach (auto privPtr, m_widgetList)
    {
        QRectF r = QRectF(privPtr->embWidgetStruct->alignPoint, privPtr->embWidgetStruct->size);
        res = r.united(res);
    }
    return res.toRect();
}


void EmbeddedPanel::autoResize()
{
    // TODO krotov Нужно как то обдумать даный функционал. Убрали для инновации.
//    int w = width();
//    int h = height();

//    foreach (EmbeddedWidgetBaseStructPrivate * item, m_widgetList)
//    {
//        w = qMax(w, item->embWidgetPtr->geometry().right() + 1);
//        h = qMax(h, item->embWidgetPtr->geometry().bottom() + 1);
//    }

//    resize(w, h);
}


void EmbeddedPanel::optimizeWidgetSizes()
{
    // Виджетов нет => делать нечего
    if(m_widgetList.empty())
    {
        return;
    }

    // FIXME использовать  ewApp()->changeWidgetSizeInGroup корректно?
    if(m_personalPanel)
    {
        QSize widgetSize = size();
        auto embPrivStruct = m_widgetList.first();

        switch(embPrivStruct->embWidgetStruct->szPolicy)
        {
          case ESP_EXPAND_FREE:
          {
              widgetSize = size();
          }
          break;

          case ESP_EXPAND_PROPORTIONAL:
          {
              double scaleH = static_cast<double>(embPrivStruct->embWidgetStruct->size.height()) / static_cast<double>(height());
              double scaleW = static_cast<double>(embPrivStruct->embWidgetStruct->size.width()) / static_cast<double>(width());
              double scale = qMax(scaleH, scaleW);
              int w = qRound((embPrivStruct->embWidgetStruct->size.width() * 1.0) / scale);
              int h = qRound((embPrivStruct->embWidgetStruct->size.height() * 1.0) / scale);

              widgetSize = QSize(static_cast<int>(w), static_cast<int>(h));
          }
          break;

          case ESP_FIXED:
          {
              widgetSize = embPrivStruct->embWidgetStruct->size;
          }
          break;
        }


        if(embPrivStruct->embWidgetStruct->size != widgetSize)
        {
            ewApp()->changeWidgetGeometry(embPrivStruct->embWidgetPtr->id(), embPrivStruct->embWidgetPtr->pos(), widgetSize);
        }
        else
        {
            ewApp()->changeWidgetGeometry(embPrivStruct->embWidgetPtr->id(), QPoint(0, 0), widgetSize);
        }

        return;
    }

    double left    = m_widgetList[0]->embWidgetStruct->alignPoint.x();
    double right   = m_widgetList[0]->embWidgetStruct->alignPoint.x() + m_widgetList[0]->embWidgetStruct->size.width();
    double top     = m_widgetList[0]->embWidgetStruct->alignPoint.y();
    double bottom  = m_widgetList[0]->embWidgetStruct->alignPoint.y() + m_widgetList[0]->embWidgetStruct->size.height();

    // Поиск крайних позиций виджетов
    for(int i = 1; i < m_widgetList.size(); ++i)
    {
        left   = qMin(left,    m_widgetList[i]->embWidgetStruct->alignPoint.x());
        right  = qMax(right,   m_widgetList[i]->embWidgetStruct->alignPoint.x() + m_widgetList[i]->embWidgetStruct->size.width());
        top    = qMin(top,     m_widgetList[i]->embWidgetStruct->alignPoint.y());
        bottom = qMax(bottom,  m_widgetList[i]->embWidgetStruct->alignPoint.y() + m_widgetList[i]->embWidgetStruct->size.height());
    }

    // Расчет коэффициента для пропорционального изменения размеров виджетов
    double scaleH = (height()*1.0) / ((bottom - top + 1)*1.0);
    double scaleW = (width()*1.0) / ((right - left + 1)*1.0);

    // Изменяем позицию и размеры каждого виджета поочередно
    for(auto embStructPrivate : m_widgetList)
    {
        int x = static_cast<int>((embStructPrivate->embWidgetPtr->x()) * scaleW);
        int y = static_cast<int>((embStructPrivate->embWidgetPtr->y()) * scaleH);
        int w = static_cast<int>((embStructPrivate->embWidgetPtr->width()) * scaleW);
        int h = static_cast<int>((embStructPrivate->embWidgetPtr->height()) * scaleH);

        ewApp()->changeWidgetGeometry(embStructPrivate->embWidgetPtr->id(), QPoint(static_cast<int>(x), static_cast<int>(y))
                                      , embStructPrivate->embWidgetPtr->size());
        ewApp()->changeWidgetSizeInGroup(embStructPrivate->embWidgetPtr->id(), QSize(static_cast<int>(w), static_cast<int>(h)));
    }
}
