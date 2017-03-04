#ifndef BLOCKDIALOG_H
#define BLOCKDIALOG_H

#include <QDialog>
#include <QMovie>
#include <QTimer>
#include <QTime>
#include <QLabel>

class BlockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BlockDialog(uint timeoutMsec, uint showTimeoutMsec);
    bool stop();

protected slots:
    void slotTimeout();
    void slotShowTimeout();

private:
    QMovie * m_Movie;
    bool m_Stop;

};

#endif
