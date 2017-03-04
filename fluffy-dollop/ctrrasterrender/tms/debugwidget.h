#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QTextBrowser>
#include <QMap>
#include <ctrcore/provider/tiledataprovider.h>

using namespace data_system;

class QVBoxLayout;
class QToolButton;

class DebugWidget : public QWidget
{
    Q_OBJECT

public:
    DebugWidget(uint loadersCount, uint queueMax2DSize, uint queueMax3DSize);
    void registerSource(uint sourceId, data_system::TileDataProvider::TmsTaskSender tmsTaskSender);
    void unregisterSource(uint sourceId);
    void setQueueSize(uint sourceId, uint size);
    void setLoadersInWork(int count);
    void setLog(const QString& url);

private:
    QVBoxLayout * _vbLayout;
    QProgressBar * _loadersPB;
    QMap<uint, QProgressBar*> _map;
    uint _queueMax2DSize, _queueMax3DSize, _queueMaxAltSize;
    QTextEdit * _log;
    QToolButton * _clearTb;
    bool _outLog;

private slots:
    void slotOutLog(bool);
    void slotReload();

signals:
    void signalShowLog();
};

#endif









