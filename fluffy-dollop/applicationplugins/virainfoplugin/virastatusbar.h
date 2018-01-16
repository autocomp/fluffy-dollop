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
class InfoForm;
class Photo360Form;
class StairOrLiftForm;

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
        AT_ALL,
        AT_RENT,
        AT_FREE,
        AT_PROP,
        AT_UNAVAILABLE
    };

public:
    explicit ViraStatusBar(quint64 parentWidgetId, QWidget *parent = 0);
    ~ViraStatusBar();

protected slots:
    void slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id);
    void slotShowMoreInfo();
    void slotMoreInfoWidgetClosed();
    void slotEditAreaGeometry(bool on_off);
    void slotAddDefect(bool on_off);
    void slotAddFoto(bool on_off);
    void slotAddFoto360(bool on_off);
    void slotAddPlaceholder(bool on_off);
    void slotAddLift(bool on_off);
    void slotAddStairs(bool on_off);
    void slotEditObjectGeometryFinish(QVariant var);
    void slotCloseMarkWindow();
    void slotObjectCenterOn(uint64_t id);
    void slotCloseMoreInfoForm();
    void showMarkInfoWidgwt(quint64 id);
    void slotMarkCreated(QVariant var);
    void slotDeleteObject();
    void slotClosePhoto360Window();
    void slotCloseStairOrLiftWindow();

protected:
    void reset();
    void showTasks(AreaData data);
    void showArendators(AreaData data, bool one);
    void showAreas(AreaData data, bool one );
    void showAddres( regionbiz::BaseAreaPtr ptr );
    void showName( regionbiz::BaseAreaPtr ptr );
    void showDebt( AreaData data );
    void showMarkName( QString name );
    void showMarkStatus( QString status );
    void showMarkWorker(QString worker );
    void showMarkCategory(QString category );
    void showMarkPriority(QString priority );
    void showMoreInfo(quint64 id);

    QString recursiveGetName(regionbiz::BaseAreaPtr area );

    QLabel* getArendator(ArendatorType type);
    QLabel* getTask( TaskType type, AreaData data );
    QLabel* getArea( AreaType type, AreaData data );

    Ui::ViraStatusBar *ui;
    quint64 _parentWidgetId;
    MarkForm * _markForm;
    EmbIFaceNotifier* _ifaceInfoMarkWidget = nullptr;
    InfoForm * _infoForm;
    EmbIFaceNotifier* _ifaceInfoWidget = nullptr;
    Photo360Form * _photo360Form;
    EmbIFaceNotifier* _ifacePhoto360Widget = nullptr;
    StairOrLiftForm * _stairOrLiftForm;
    EmbIFaceNotifier* _ifaceStairOrLiftWidget = nullptr;
};

#endif // VIRASTATUSBAR_H
