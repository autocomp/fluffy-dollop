#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QMovie>

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(bool& cancelFlag, const QString & captionText, const QString & cancelButtonText);
    void closeBlockDialog();

public slots:
    void setPersent(uint persent);

protected slots:
    void slotCancel();

private:
    bool& m_cancelFlag;
    QProgressBar * m_progressBar;
    QPushButton * m_cancelButton;

signals:
    void signalCancel();
};

class WaitDialogUnlimited : public QDialog
{
    Q_OBJECT
public:
    WaitDialogUnlimited( QString descriptionText = "", QString userButtonText = "", uint maxWgtWidth = 150,
                         QWidget *parent = 0, QString gifResourcePath = ":/img/loading.gif" );
    ~WaitDialogUnlimited();

private:
    void startWaiting();
    void stopWaiting();

signals:
    void sigUserButtonClicked();

private:
    QMovie * m_movie;
};


#endif
