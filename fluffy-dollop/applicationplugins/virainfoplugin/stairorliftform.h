#ifndef STAIRORLIFTFORM_H
#define STAIRORLIFTFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <regionbiz/rb_manager.h>

namespace Ui {
class StairOrLiftForm;
}

class FloorTreeWidgetItem : public QTreeWidgetItem
{
public:
    uint64_t _id;
    uint64_t _number;

    FloorTreeWidgetItem(QString name, uint64_t id, uint64_t number)
        : QTreeWidgetItem(QStringList() << name)
        , _id(id)
        , _number(number)
    {
        setCheckState(0, Qt::Checked);
    }
};

class StairOrLiftForm : public QWidget
{
    Q_OBJECT

public:
    explicit StairOrLiftForm();
    ~StairOrLiftForm();
    void createEntity(uint64_t facilityId, regionbiz::Mark::MarkType type, QPointF pos);
    void editEntity(uint64_t entityId, uint64_t facilityId);

signals:
    void signalCloseWindow();

public slots:
    void slotApply();
    void slotCloseWindow();

private:
    void fillFloorsList(regionbiz::FacilityPtr facilityPtr);

    Ui::StairOrLiftForm *ui;
    uint64_t _facilityId;
    regionbiz::Mark::MarkType _type = regionbiz::Mark::MT_INVALID;
    QPointF _pos;

};

#endif // STAIRORLIFTFORM_H
