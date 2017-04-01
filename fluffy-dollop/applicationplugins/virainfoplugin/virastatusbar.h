#ifndef VIRASTATUSBAR_H
#define VIRASTATUSBAR_H

#include <QWidget>
#include <QLabel>

#include <ctrcore/plugin/embifacenotifier.h>
#include <regionbiz/rb_manager.h>

#include "data_getter.h"

namespace Ui {
class ViraStatusBar;
}

class MarkForm;

class ViraStatusBar : public QWidget
{
    Q_OBJECT

    enum TaskType
    {
        TT_WORK,
        TT_CHECK,
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
    void slotEditAreaGeometry(bool on_off);
    void slotAddMark(bool on_off);
    void slotEditObjectGeometryFinish(QVariant var);
    void slotMarkWidgetClosed();
    void slotCloseMarkWindow();
    void slotObjectCenterOn(uint64_t id);

protected:
    void reset();
    void showTasks(AreaData data);
    void showArendators(AreaData data, bool one);
    void showAreas(AreaData data, bool one );
    void showAddres( regionbiz::BaseAreaPtr ptr );
    void showName( regionbiz::BaseAreaPtr ptr );
    void showDebt( AreaData data );
    void showMarkName( QString name );
    void showMarkInfoWidgwt(bool isEditMode, qulonglong id);
    void showMarkStatus( QString status );
    void showMarkWorker(QString worker );
    void showMarkCategory(QString category );
    void showMarkPriority(QString priority );

    QString recursiveGetName(regionbiz::BaseAreaPtr area );

    QLabel* getArendator(ArendatorType type);
    QLabel* getTask( TaskType type, AreaData data );
    QLabel* getArea( AreaType type, AreaData data );

    Ui::ViraStatusBar *ui;
    quint64 _parentWidgetId;
    EmbIFaceNotifier* _ifaceInfoAreaWidget = nullptr;
    MarkForm * _markForm;
    EmbIFaceNotifier* _ifaceInfoMarkWidget = nullptr;
};

#endif // VIRASTATUSBAR_H
