#include "embeddedapp.h"
#include "embeddedsubiface.h"
#include <QDebug>

#ifdef QT_V5
#include <QWindow>
#endif

#include <QProcess>
#include <QThread>

using namespace ew;
EmbeddedSubIFace::EmbeddedSubIFace()
{
}


EmbeddedSubIFace::~EmbeddedSubIFace()
{
}


quint64 EmbeddedSubIFace::id()
{
    return ewApp()->getWidgetIdBySubIFacePtr(this);
}


#ifdef QT_V5


quint64 EmbeddedSubIFace::getWID()
{
    return m_wId;
}


QWindow *EmbeddedSubIFace::getWindow()
{
    return m_pExtWindow;
}


#endif


EmbeddedFace::EmbeddedFace(QWidget *wdg) :
    EmbeddedSubIFace()
{
    m_pWidget = wdg;
}


quint64 EmbeddedFace::getWidByPid(quint64 pid)
{
#ifdef Q_OS_UNIX
    QList<quint64> wids;
    QString cmdXWin = QString("xwininfo -root -children -all | awk \'{print($1)}\'");
    QProcess *procXWin = new QProcess();
    procXWin->start("/bin/sh", QStringList() << "-c" << cmdXWin);
    procXWin->waitForFinished();
    QString fullOut = QString(procXWin->readAll());
    fullOut = fullOut.remove("xwininfo:");
    delete procXWin;
    QStringList lst = fullOut.split("\n");
    foreach (QString winIdStr, lst)
    {
        QString cmdXProp = QString("xprop -id ") + winIdStr
                           + " | /bin/grep \"_NET_WM_PID(CARDINAL) = \" | /bin/awk \'{print($3)}\' ";
        QProcess *proc1 = new QProcess();

        proc1->start("/bin/sh", QStringList() << "-c" << cmdXProp);
        proc1->waitForFinished();
        QString fullOut1 = QString(proc1->readAll());
        quint64 curPid = fullOut1.toULongLong();

        if(pid == curPid)
        {
            wids.append(winIdStr.toULongLong(0, 16));
        }

        delete proc1;
    }

    if(wids.isEmpty())
    {
        qDebug() << "EmbeddedFace::getWidByPid: external window ID not found!";
        return 0;
    }

    if(wids.size() > 1)
    {
        qDebug() << "EmbeddedFace::getWidByPid: Warn! Found " << wids << " window id's. Use last";
        return wids.last();
    }
    else
    {
        return wids.first();
    }

#endif


    return 0;
}


void EmbeddedFace::restore(QMap<QString, QVariant> propMap)
{
    emit signalRestore(propMap);
}


void EmbeddedFace::closed()
{
    emit signalClosed();
}


void EmbeddedFace::visisbleChanged(bool vis)
{
    emit signalVisibleChanged(vis);
}


#ifdef QT_V5
#include <QApplication>
#include <QEventLoop>
#include <QTime>
EmbeddedFace::EmbeddedFace(QString appStr, QStringList args, QProcessEnvironment env ) :
    EmbeddedSubIFace()
{
    QProcess *proc = new QProcess();

    proc->setProcessEnvironment(env);
    proc->start(appStr, args);
    proc->waitForStarted();
    QTime tm;
    tm.start();

    while(tm.elapsed() < 1000)
    {
        qApp->processEvents();
    }

    qint64 pid = proc->processId();

    /*если тут дать wid существующего окна, то оно встраивается более-менее корректно.*/
    quint64 wid = getWidByPid(pid);
    QWindow *wind = 0;

    if(0 != wid)
    {
        wind = QWindow::fromWinId(wid);
        m_wId = wid;
        m_pExtWindow = wind;
    }

    m_pWidget = QWidget::createWindowContainer(wind);
}


EmbeddedFace::EmbeddedFace(quint64 wId) :
    EmbeddedSubIFace()
{
    QWindow *wind = 0;

    if(0 != wId)
    {
        QWindow * parentWnd = new QWindow();
        wind = QWindow::fromWinId(wId);
        wind->setParent(parentWnd);
        m_wId = wId;
        m_pExtWindow = parentWnd;
        m_pWidget = QWidget::createWindowContainer(wind);
    }
    else
    {
        m_pWidget = nullptr;
    }
}


#endif


QWidget *EmbeddedFace::getWidget()
{
    return m_pWidget;
}
