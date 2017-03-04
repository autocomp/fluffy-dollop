#ifndef BLOCKDIALOG_H
#define BLOCKDIALOG_H

#include <QDialog>
#include <QProgressBar>

namespace visualize_system {

class BlockDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BlockDialog(bool & cancelFlag);
    void setPercent(int percent);

protected slots:
    void slotCancel();

private:
    bool & m_cancelFlag;
    QProgressBar * m_ProgressBar;

signals:
    void signalCancel();
};

}

#endif
