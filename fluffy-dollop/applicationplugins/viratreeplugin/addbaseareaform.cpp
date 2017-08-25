#include "addbaseareaform.h"
#include "ui_addbaseareaform.h"

using namespace regionbiz;

AddBaseAreaForm::AddBaseAreaForm(regionbiz::BaseArea::AreaType type, uint64_t id)
    : ui(new Ui::AddBaseAreaForm)
    , ew::EmbeddedSubIFace()
    , _type(type)
    , _id(id)
{
    QString name;
    switch(_type)
    {
    case BaseArea::AT_REGION : name = QString::fromUtf8("регион"); break;
    case BaseArea::AT_LOCATION : name = QString::fromUtf8("локация"); break;
    case BaseArea::AT_FACILITY : name = QString::fromUtf8("здание"); break;
    case BaseArea::AT_FLOOR : name = QString::fromUtf8("этаж"); break;
    case BaseArea::AT_ROOM : name = QString::fromUtf8("комната"); break;
    }

    ui->setupUi(this);
    ui->name->setText(name);
    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    ui->apply->setIcon(QIcon(":/img/ok_button.png"));

    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(slotClose()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(slotApply()));
}

AddBaseAreaForm::~AddBaseAreaForm()
{
    delete ui;
}

QWidget *AddBaseAreaForm::getWidget()
{
    return this;
}

void AddBaseAreaForm::slotApply()
{
    BaseAreaPtr createdPtr = RegionBizManager::instance()->addArea(_type, _id);
    if(createdPtr)
    {
        createdPtr->setName(ui->name->text());
        QString description = ui->description->toPlainText();
        if( ! description.isEmpty())
            createdPtr->setDesription(ui->description->toPlainText());
        createdPtr->commit();
    }

    slotClose();
}

void AddBaseAreaForm::slotClose()
{
    ewApp()->setVisible(id(), false);
}






