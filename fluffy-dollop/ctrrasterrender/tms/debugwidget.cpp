#include "debugwidget.h"
#include "tileloaderpool.h"
#include <ctrcore/provider/timagecache.h>

#include <QGroupBox>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QTextBrowser>
#include <QToolButton>

DebugWidget::DebugWidget(uint loadersCount, uint queueMax2DSize, uint queueMax3DSize)
    : QWidget(0, Qt::WindowStaysOnTopHint)
    , _queueMax2DSize(queueMax2DSize)
    , _queueMax3DSize(queueMax3DSize)
    , _queueMaxAltSize(10000)//queueMaxAltSize)
    , _outLog(false)
{
    setWindowTitle(QString::fromUtf8("Отладка ТМС-пула"));

    QVBoxLayout * _mainVLayout = new QVBoxLayout(this);

    _vbLayout = new QVBoxLayout(this);

    QLabel * lb = new QLabel(this);
    lb->setText(QString::fromUtf8("Количество загрузчиков : ") + QString::number(loadersCount));
    _vbLayout->addWidget(lb);

    lb = new QLabel(this);
    lb->setText(QString::fromUtf8("Макс. размер 2D очередей : ") + QString::number(queueMax2DSize));
    _vbLayout->addWidget(lb);

    lb = new QLabel(this);
    lb->setText(QString::fromUtf8("Макс. размер 3D очередей : ") + QString::number(queueMax3DSize));
    _vbLayout->addWidget(lb);

    lb = new QLabel(this);
    lb->setText(QString::fromUtf8("Размер кеша в тайлах : ") + QString::number(TImageCache::instance()->cacheLimit()));
    _vbLayout->addWidget(lb);

    QFrame * fr = new QFrame(this);
    fr->setFrameShape(QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken );
    _vbLayout->addWidget(fr);

    lb = new QLabel(this);
    lb->setText(QString::fromUtf8("Загрузчики в работе :"));
    _vbLayout->addWidget(lb);

    _loadersPB = new QProgressBar(this);
    _loadersPB->setRange(0, loadersCount);
    _loadersPB->setTextVisible(true);
    _loadersPB->setValue(0);
    _loadersPB->setFormat("%v");
    _vbLayout->addWidget(_loadersPB);

    fr = new QFrame(this);
    fr->setFrameShape(QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken );
    _vbLayout->addWidget(fr);

    lb = new QLabel(this);
    lb->setText(QString::fromUtf8("Очереди загрузки :"));
    _vbLayout->addWidget(lb);

    _mainVLayout->addLayout(_vbLayout);

    QSpacerItem* spacer = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Maximum);
    _mainVLayout->addSpacerItem(spacer);

    fr = new QFrame(this);
    fr->setFrameShape(QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken );
    _mainVLayout->addWidget(fr);

    _log = new QTextEdit(this);
    _log->setReadOnly(true);
    _log->hide();

    QHBoxLayout * hbLayout = new QHBoxLayout(this);

    QToolButton * tb = new QToolButton(this);
    connect(tb, SIGNAL(clicked()), this, SLOT(slotReload()));
    tb->setToolTip(QString::fromUtf8("Перерисовать экран"));
    tb->setIcon(QIcon(":/263_step_forth.png"));
    tb->setFixedSize(32,32);
    tb->setIconSize(QSize(32,32));
    tb->setAutoRaise(true);
    hbLayout->addWidget(tb, 0, Qt::AlignLeft);

    fr = new QFrame(this);
    fr->setFrameShape(QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken );
    hbLayout->addWidget(fr, 0, Qt::AlignLeft);

    tb = new QToolButton(this);
    connect(tb, SIGNAL(toggled(bool)), this, SLOT(slotOutLog(bool)));
    tb->setToolTip(QString::fromUtf8("Выводить лог"));
    tb->setIcon(QIcon(":/95_edit.png"));
    tb->setFixedSize(32,32);
    tb->setIconSize(QSize(32,32));
    tb->setAutoRaise(true);
    tb->setCheckable(true);
    hbLayout->addWidget(tb, 0, Qt::AlignLeft);

    _clearTb = new QToolButton(this);
    connect(_clearTb, SIGNAL(clicked()), _log, SLOT(clear()));
    _clearTb->setToolTip(QString::fromUtf8("Очистить лог"));
    _clearTb->setIcon(QIcon(":/268_purge.png"));
    _clearTb->setFixedSize(32,32);
    _clearTb->setIconSize(QSize(32,32));
    _clearTb->setAutoRaise(true);
    _clearTb->hide();
    hbLayout->addWidget(_clearTb, 0, Qt::AlignLeft);

    spacer = new QSpacerItem(500, 0, QSizePolicy::Preferred, QSizePolicy::Minimum);
    hbLayout->addSpacerItem(spacer);

    _mainVLayout->addLayout(hbLayout);

    _mainVLayout->addWidget(_log);

    update();
}

void DebugWidget::setQueueSize(uint sourceId, uint size)
{
    QMap<uint, QProgressBar*>::iterator it(_map.find(sourceId));
    if(it != _map.end())
    {
        it.value()->setValue(size);
        update();
    }
}

void DebugWidget::setLog(const QString& url)
{
    if(_outLog)
        _log->append(url);
}

void DebugWidget::registerSource(uint sourceId, TileDataProvider::TmsTaskSender tmsTaskSender)
{
    QProgressBar * pb = new QProgressBar(this);
    uint maxQueue;
    switch (tmsTaskSender) {
    case TileDataProvider::TmsMapper2D : maxQueue = _queueMax2DSize;         pb->setFormat("%v,2D"); break;
    case TileDataProvider::TmsMapper3D : maxQueue = _queueMax3DSize;         pb->setFormat("%v,3D"); break;
    case TileDataProvider::AltitudeController : maxQueue = _queueMaxAltSize; pb->setFormat("%v,H"); break;
    }
    pb->setTextVisible(true);
    pb->setValue(0);
    pb->setRange(0, maxQueue);
    _vbLayout->addWidget(pb);
    _map.insert(sourceId, pb);
    update();
}

void DebugWidget::unregisterSource(uint sourceId)
{
    QMap<uint, QProgressBar*>::iterator it(_map.find(sourceId));
    if(it != _map.end())
    {
        delete it.value();
        update();
        _map.erase(it);
        update();
    }
}

void DebugWidget::setLoadersInWork(int count)
{
    _loadersPB->setValue(count);
}

void DebugWidget::slotOutLog(bool on_off)
{
    _outLog = on_off;
    _log->setVisible(on_off);
    _clearTb->setVisible(on_off);
    if(on_off)
        emit signalShowLog();
}

void DebugWidget::slotReload()
{
    TImageCache::instance()->clear();
    TileLoaderPool::instance()->emit_reload();
}























