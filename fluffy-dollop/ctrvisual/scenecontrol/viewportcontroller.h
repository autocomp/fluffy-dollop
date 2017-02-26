#ifndef VIEWPORTCONTROLLER_H
#define VIEWPORTCONTROLLER_H

#include <QObject>
#include <QString>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include "cell.h"
#include <QTime>

namespace visualize_system {

class SceneController;

class ViewPortController : public QObject
{
    Q_OBJECT

    struct ReceivedImage
    {
        ReceivedImage(const QPixmap& _pixmap, const QString& _url) : pixmap(_pixmap), url(_url) {}
        QPixmap pixmap;
        QString url;
    };

public:
    ViewPortController(SceneController *const sceneController);
    ~ViewPortController();
    void updateContent(bool deepRepaint);
    void updateContentForSave(double Z);
    int zoomLevel() {return m_zoom;}

    bool m_chenged;

public slots:
    void update(int zoom, QRectF visibleRect, bool deleteUnvisibleCells);
    void receivedImage(const QPixmap &, const QString& url);

protected slots:
//    void timerTimeout();
    void loadingFinished();

private:
    SceneController * const m_SceneController;
    int m_zoom;
    int64_t m_leftX, m_rightX, m_topY, m_bottomY;
    QMap<QString,Cell> m_cellMap;
    QRectF m_lastViewRect, m_viewOriginalRect;
    QRect m_lastViewRectInTiles;
    QList<Cell> m_cellForDeleting;
    QList<ReceivedImage> m_ReceivedImages;
    bool m_updateContent;

    void addCell(QList<Cell>& cellList);
    void deleteCell(QList<Cell>& cellList);
    bool getVisibleCells(int zoom, QRectF viewportRect, QList<Cell>& cellList);
    void addVisibleCellsToList(int zoom, QRectF viewportRect, QList<Cell>& cellList);
    void localUpdate(int zoom, QRectF visibleRect, bool deleteUnvisibleCells);
    void drawImage(const QPixmap& pixmap, const QString& url);

signals:
    void setViewEnabled(bool);
    void signalUpdateBackgroundItem(QRect);
};
}
#endif





