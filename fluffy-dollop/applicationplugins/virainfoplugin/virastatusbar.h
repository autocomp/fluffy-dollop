#ifndef VIRASTATUSBAR_H
#define VIRASTATUSBAR_H

#include <QWidget>
#include <ctrcore/plugin/embifacenotifier.h>

namespace Ui {
class ViraStatusBar;
}

class ViraStatusBar : public QWidget
{
    Q_OBJECT

public:
    explicit ViraStatusBar(quint64 parentWidgetId, QWidget *parent = 0);
    ~ViraStatusBar();

protected slots:
    void slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id);
    void slotShowMoreInfo(bool on_off);
    void slotMoreInfoWidgetClosed();

protected:
    void reset();

    Ui::ViraStatusBar *ui;
    quint64 _parentWidgetId;
    EmbIFaceNotifier* _iface = nullptr;
};

#endif // VIRASTATUSBAR_H
