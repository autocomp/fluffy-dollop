#ifndef VIRASTATUSBAR_H
#define VIRASTATUSBAR_H

#include <QWidget>
#include <QLabel>

#include <ctrcore/plugin/embifacenotifier.h>
#include <regionbiz/rb_manager.h>

#include "test_data_getter.h"

namespace Ui {
class ViraStatusBar;
}

class ViraStatusBar : public QWidget
{
    Q_OBJECT

    enum TaskType
    {
        TT_WORK,
        TT_LOST,
        TT_NEW
    };

    enum ArendatorType
    {
        AT_GOOD,
        AT_BAD
    };

    enum AreaType
    {
        AT_RENT,
        AT_FREE,
        AT_PROP
    };

public:
    explicit ViraStatusBar(quint64 parentWidgetId, QWidget *parent = 0);
    ~ViraStatusBar();

protected slots:
    void slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id);
    void slotShowMoreInfo(bool on_off);
    void slotMoreInfoWidgetClosed();

protected:
    void reset();
    void showTasks(TestData data);
    void showArendators(TestData data, bool one);
    void showAreas(TestData data, bool one );
    void showAddres( regionbiz::BaseAreaPtr ptr );
    void showName( regionbiz::BaseAreaPtr ptr );
    void showDebt( TestData data );

    QLabel* getArendator(ArendatorType type);
    QLabel* getTask( TaskType type, TestData data );
    QLabel* getArea( AreaType type, TestData data );

    Ui::ViraStatusBar *ui;
    quint64 _parentWidgetId;
    EmbIFaceNotifier* _iface = nullptr;
};

#endif // VIRASTATUSBAR_H
