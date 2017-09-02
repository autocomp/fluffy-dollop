#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QMovie>
#include <ctrcore/plugin/embifacenotifier.h>

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(const QString & captionText);
    ~ProgressDialog();

public slots:
    void setPersent(uint persent);

protected slots:
    void slotEmbWidgetClose();

private:
    QLabel * m_label;
    QProgressBar * m_progressBar;
    EmbIFaceNotifier* _iface;
};

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(bool& cancelFlag, const QString & captionText, const QString & cancelButtonText);
    ~WaitDialog();
    void closeBlockDialog();
    void setCancelButtonVisibility(bool on_off);

public slots:
    void setPersent(uint persent);

protected slots:
    void slotCancel();
    void slotEmbWidgetClose();

private:
    bool& m_cancelFlag;
    QLabel * m_label;
    QProgressBar * m_progressBar;
    QPushButton * m_cancelButton;
    EmbIFaceNotifier* _iface;

signals:
    void signalCancel();
};

class WaitDialogUnlimited : public QDialog
{
    Q_OBJECT
public:
    WaitDialogUnlimited( QString descriptionText = "", QString userButtonText = "", uint maxWgtWidth = 150,
                         QWidget *parent = 0, QString gifResourcePath = QString("://img/loading.gif") );
    ~WaitDialogUnlimited();

private:
    void startWaiting();
    void stopWaiting();

private slots:
    void slotEmbWidgetClose();

signals:
    void sigUserButtonClicked();

private:
    QMovie * m_movie;
    EmbIFaceNotifier* _iface;
};


#endif
