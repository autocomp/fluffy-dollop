#include "infoform.h"
#include "ui_infoform.h"
#include <QFileDialog>

using namespace regionbiz;

InfoForm::InfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoForm)
{
    ui->setupUi(this);

    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    ui->apply->setIcon(QIcon(":/img/ok_button.png"));
    ui->loadImage->setIcon(QIcon(":/img/add_image.png"));

    QStringList list;
    list << QString::fromUtf8("Свободно") << QString::fromUtf8("В аренде") << QString::fromUtf8("Недоступно");
    ui->status->addItems(list);

    connect(ui->marksTreeWidget->header(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderSectionClicked(int)));
    ui->marksTreeWidget->header()->setSortIndicatorShown(true);
    ui->marksTreeWidget->setSortingEnabled(true);
    ui->marksTreeWidget->sortByColumn(0, Qt::AscendingOrder);

    connect(ui->marksTreeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotDoubleClickOnMark(QTreeWidgetItem*,int)));


    connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(signalCloseWindow()));
    connect(ui->loadImage, SIGNAL(clicked()), this, SLOT(slotLoadImage()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(slotApply()));

    ui->tasksNew->setText("0");
    ui->tasksNew->setToolTip(QString::fromUtf8("Новые"));
    QString     color = "#FF8C00";
    ui->tasksNew->setStyleSheet( "background-color: " + color + ";"
                                                                "border-color: rgb(255, 255, 255);"
                                                                "border-radius: 10px;"
                                                                "color: rgb(0, 0, 0);"
                                                                "border-width: 2px;"
                                                                "border-style: solid;" );
    ui->tasksInWork->setText("0");
    ui->tasksInWork->setToolTip(QString::fromUtf8("В работе"));
    color = "#F0E68C";
    ui->tasksInWork->setStyleSheet( "background-color: " + color + ";"
                                                                "border-color: rgb(255, 255, 255);"
                                                                "border-radius: 10px;"
                                                                "color: rgb(0, 0, 0);"
                                                                "border-width: 2px;"
                                                                "border-style: solid;" );

    ui->tasksDone->setText("0");
    ui->tasksDone->setToolTip(QString::fromUtf8("На проверку"));
    color = "#3CE63C";
    ui->tasksDone->setStyleSheet( "background-color: " + color + ";"
                                                                "border-color: rgb(255, 255, 255);"
                                                                "border-radius: 10px;"
                                                                "color: rgb(0, 0, 0);"
                                                                "border-width: 2px;"
                                                                "border-style: solid;" );
}

InfoForm::~InfoForm()
{
    delete ui;
}

void InfoForm::showWidget(quint64 id)
{
    _id = 0;
    _tasks_new = 0;
    _tasks_in_work = 0;
    _tasks_for_check = 0;

    ui->name->clear();
    ui->renter->clear();
    ui->square->setValue(0);
    ui->marksTreeWidget->clear();
    ui->description->clear();

    ui->tasksNew->setText("0");
    ui->tasksInWork->setText("0");
    ui->tasksDone->setText("0");

    ui->apply->setDisabled(true);
    ui->loadImage->setDisabled(true);

    BaseAreaPtr area = RegionBizManager::instance()->getBaseArea(id);
    if( ! area) return;

    _id = id;
    ui->apply->setDisabled(false);
    ui->loadImage->setDisabled(false);

    ui->name->setText(recursiveGetName(area));

    switch( area->getType() )
    {
    case BaseArea::AT_LOCATION:
    case BaseArea::AT_FACILITY:
    case BaseArea::AT_FLOOR:
        ui->roomWidget->hide();
        break;
    case BaseArea::AT_ROOMS_GROUP:
    case BaseArea::AT_ROOM:
    {
        RoomPtr room = BaseArea::convert< Room >( area );
        if(room)
        {
            BaseMetadataPtr areaPtr = room->getMetadata("area");
            if(areaPtr)
            {
                double area = areaPtr->getValueAsVariant().toDouble();
                ui->square->setValue(area);
            }

            BaseMetadataPtr rentorPtr = room->getMetadata("rentor");
            if(rentorPtr)
                ui->renter->setText(rentorPtr->getValueAsVariant().toString());

            BaseMetadataPtr statusPtr = room->getMetadata("status");
            if(statusPtr)
            {
                QString status = statusPtr->getValueAsVariant().toString();
                if(status == QString::fromUtf8("Свободно"))
                    ui->status->setCurrentText(QString::fromUtf8("Свободно"));
                else if(status == QString::fromUtf8("В аренде"))
                    ui->status->setCurrentText(QString::fromUtf8("В аренде"));
                else
                    ui->status->setCurrentText(QString::fromUtf8("Недоступно"));
            }

            BaseMetadataPtr commentPtr = room->getMetadata("comment");
            if(commentPtr)
                ui->description->setText(commentPtr->getValueAsVariant().toString());
        }
        ui->roomWidget->show();
        break;
    }
    }

    QString facility, floor;
    switch( area->getType() )
    {
    case BaseArea::AT_LOCATION:{
        LocationPtr locationPtr = BaseArea::convert< Location >( area );
        if(locationPtr)
        {
            std::vector< FacilityPtr > facilities = locationPtr->getChilds();
            for( FacilityPtr facilityPtr: facilities )
                loadTasks(facilityPtr->getDescription(), "", facilityPtr);
        }
        }break;

    case BaseArea::AT_FACILITY:
        facility = area->getDescription();
        loadTasks(facility, "", area);
        break;

    case BaseArea::AT_FLOOR:
        getFloorFullName(facility, area);
        loadTasks(facility, area->getName(), area);
        break;

    case BaseArea::AT_ROOMS_GROUP:
    case BaseArea::AT_ROOM:
        getRoomFullName(facility, floor, area);
        loadTasks(facility, floor, area);
        break;
    }

    ui->tasksNew->setText(QString::number(_tasks_new));
    ui->tasksInWork->setText(QString::number(_tasks_in_work));
    ui->tasksDone->setText(QString::number(_tasks_for_check));
}

void InfoForm::loadTasks(const QString &facilityName, const QString &floorName, regionbiz::BaseAreaPtr area)
{
    switch( area->getType() )
    {
    case BaseArea::AT_FACILITY:{
        FacilityPtr facilityPtr = BaseArea::convert< Facility >( area );
        if( ! facilityPtr) return;
        FloorPtrs floors = facilityPtr->getChilds();
        for( FloorPtr floorPtr: floors )
        {
            loadTasks(facilityName, floorPtr->getName(), floorPtr);
        }
    }break;

    case BaseArea::AT_FLOOR:{
        FloorPtr floorPtr = BaseArea::convert< Floor >( area );
        if( ! floorPtr) return;
        BaseAreaPtrs rooms = floorPtr->getChilds( Floor::FCF_ALL_ROOMS );
        for( BaseAreaPtr room_ptr: rooms )
        {
            RoomPtr room = BaseArea::convert< Room >( room_ptr );
            if(room)
                loadTasks(facilityName, floorName, room);
        }
    }break;

    case BaseArea::AT_ROOMS_GROUP:
    case BaseArea::AT_ROOM:{
        RoomPtr room = BaseArea::convert< Room >( area );
        if(room)
        {
            MarkPtrs marks_of_room = room->getMarks();
            for( MarkPtr mark: marks_of_room )
                loadTasks(facilityName, floorName, room->getName(), mark);
        }
    }break;
    }
}

void InfoForm::loadTasks(const QString &facilityName, const QString &floorName, const QString &roomName, regionbiz::MarkPtr ptr)
{
    QStringList list;
    list << facilityName << floorName << roomName << ptr->getName();

    BaseMetadataPtr worker = ptr->getMetadata("worker");
    if(worker)
        list << worker->getValueAsVariant().toString();
    else
        list << QString();

    BaseMetadataPtr date = ptr->getMetadata("date");
    if(date)
        list << date->getValueAsVariant().toString();
    else
        list << QString();

    BaseMetadataPtr priority = ptr->getMetadata("priority");
    if(priority)
        list << priority->getValueAsVariant().toString();
    else
        list << QString();

    BaseMetadataPtr status = ptr->getMetadata("status");
    if(status)
    {
        QString statusStr = status->getValueAsVariant().toString();
        list << statusStr;

        if(statusStr == QString::fromUtf8("новый"))
            ++_tasks_new;
        else if(statusStr == QString::fromUtf8("в работе"))
            ++_tasks_in_work;
        else if(statusStr == QString::fromUtf8("на проверку"))
            ++_tasks_for_check;
    }
    else
        list << QString();

    BaseMetadataPtr category = ptr->getMetadata("category");
    if(category)
        list << category->getValueAsVariant().toString();
    else
        list << QString();

    QTreeWidgetItem * item = new QTreeWidgetItem(ui->marksTreeWidget, list);
}

void InfoForm::slotLoadImage()
{
    QStringList list = QFileDialog::getOpenFileNames(this);
    foreach (QString name, list)
    {
        QPixmap pm(name);
        if(pm.isNull() == false)
        {
//            _pixmaps.append(pm);
//            _listWidget->addItem(pm);
        }
    }
}

void InfoForm::slotApply()
{
//    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
//    if(ptr)
//    {
//        QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
//        if(regionBizInitJson_Path.isValid())
//        {
//            QString destPath = regionBizInitJson_Path.toString();
//            QDir dir(destPath);
//            dir.mkdir(QString::number(_id));
//            destPath = destPath + QDir::separator() + QString::number(_id) + QDir::separator();
//            int N(0);
//            foreach(QPixmap pm, _pixmaps)
//                pm.save(destPath + QString::number(QDateTime::currentMSecsSinceEpoch()) + QString("_") + QString::number(++N) + ".tiff");
//        }

//        ptr->setName( ui->defect->text());
//        ptr->addMetadata("string", "worker", ui->responsible->text());
//        ptr->setDesription( ui->description->toPlainText());
//        QString dataStr = ui->dateEdit->date().toString("dd.MM.yy");
//        bool res = ptr->addMetadata("string", "date", dataStr);
//        ptr->addMetadata("string", "priority", ui->importance->currentText());
//        ptr->addMetadata("string", "category", ui->category->currentText());
//        if(moveToArchive)
//            ptr->addMetadata("string", "status", QString::fromUtf8("в архиве"));
//        else
//            ptr->addMetadata("string", "status", ui->status->currentText());

//        bool commitRes = ptr->commit();
//        qDebug() << _id << ", dataStr:" << dataStr << res << ", commitRes:" << commitRes;
//    }
    emit signalCloseWindow();
}

void InfoForm::getFloorFullName(QString &facility, regionbiz::BaseAreaPtr area)
{
    BaseAreaPtr parentArea = area->getParent();
    if(parentArea)
        facility = parentArea->getDescription();
}

void InfoForm::getRoomFullName(QString &facility, QString &floor, regionbiz::BaseAreaPtr area)
{
    BaseAreaPtr parentArea = area->getParent();
    if(parentArea)
    {
        floor = parentArea->getName();
        BaseAreaPtr grandParentArea = parentArea->getParent();
        if(grandParentArea)
            facility = grandParentArea->getDescription();
    }
}

QString InfoForm::recursiveGetName(regionbiz::BaseAreaPtr area)
{
    QString name;
    switch( area->getType() )
    {
    case BaseArea::AT_LOCATION:
    case BaseArea::AT_FACILITY:
    {
        name = area->getDescription();
        break;
    }
    case BaseArea::AT_REGION:
    case BaseArea::AT_FLOOR:
    case BaseArea::AT_ROOMS_GROUP:
    case BaseArea::AT_ROOM:
    {
        name = area->getName();
        break;
    }
    }

    if( BaseArea::AT_REGION == area->getType() )
        return name;
//    if( BaseArea::AT_FLOOR == area->getType() )
//        name = '\n' + name;

    QString that_name = recursiveGetName( area->getParent() ) + " / " + name;
    return  that_name;
}

void InfoForm::slotHeaderSectionClicked(int index)
{
    ui->marksTreeWidget->sortByColumn(index);
}

void InfoForm::slotDoubleClickOnMark(QTreeWidgetItem *, int)
{



}
