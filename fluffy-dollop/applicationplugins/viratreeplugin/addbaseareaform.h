#ifndef ADDBASEAREAFORM_H
#define ADDBASEAREAFORM_H

#include <QWidget>
#include <libembeddedwidgets/embeddedapp.h>
#include <regionbiz/rb_manager.h>

namespace Ui {
class AddBaseAreaForm;
}

class AddBaseAreaForm : public QWidget, public ew::EmbeddedSubIFace
{
    Q_OBJECT

public:
    AddBaseAreaForm(regionbiz::BaseArea::AreaType type, uint64_t id);
    ~AddBaseAreaForm();
    virtual QWidget *getWidget();

signals:
    void signalCloseWindow();

private slots:
    void slotApply();
    void slotClose();
    void slotLoadPdf();
    void slotFloorOrRoomCountChanged(int);

private:
    Ui::AddBaseAreaForm *ui;
    const regionbiz::BaseArea::AreaType _type;
    const uint64_t _id;
    QString _name, _pdfFileName;

};

#endif // ADDBASEAREAFORM_H
