#include "infoform.h"
#include "ui_infoform.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <QFileDialog>
#include <QImageReader>

using namespace regionbiz;

InfoForm::InfoForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InfoForm)
{
    ui->setupUi(this);

    _listWidget = new PixmapListWidget;
    ui->galeryLayout->addWidget(_listWidget);

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
    delete _listWidget;
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
    _listWidget->clear();
    _pixmaps.clear();

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
                ui->square->setProperty("area", QString::number(area, 'f', 1));
            }

            BaseMetadataPtr rentorPtr = room->getMetadata("rentor");
            if(rentorPtr)
            {
                QString rentorStr = rentorPtr->getValueAsVariant().toString();
                ui->renter->setText(rentorStr);
                ui->renter->setProperty("rentor", rentorStr);
            }

            BaseMetadataPtr statusPtr = room->getMetadata("status");
            if(statusPtr)
            {
                QString statusStr = statusPtr->getValueAsVariant().toString();
                if(statusStr == QString::fromUtf8("Свободно"))
                    ui->status->setCurrentText(QString::fromUtf8("Свободно"));
                else if(statusStr == QString::fromUtf8("В аренде"))
                    ui->status->setCurrentText(QString::fromUtf8("В аренде"));
                else
                    ui->status->setCurrentText(QString::fromUtf8("Недоступно"));

                ui->status->setProperty("status", statusStr);
            }

            BaseMetadataPtr commentPtr = room->getMetadata("comment");
            if(commentPtr)
            {
                QString commentStr = commentPtr->getValueAsVariant().toString();
                ui->description->setText(commentStr);
                ui->description->setProperty("comment", commentStr);
            }
        }
        ui->roomWidget->show();
        break;
    }
    }
    reloadTasks(area);

    QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
    if(regionBizInitJson_Path.isValid())
    {
         QString destPath = regionBizInitJson_Path.toString() + QDir::separator() + QString::number(_id);
         QDir dir(destPath);
         QStringList list = dir.entryList(QDir::Files);
         foreach(QString fileName,list)
         {
             fileName.prepend(destPath + QDir::separator());
             QPixmap pm(fileName);
             if(pm.isNull() == false)
                 _listWidget->addItem(pm);
         }
    }
}

void InfoForm::reloadTasks(regionbiz::BaseAreaPtr area)
{
    _tasks_new = 0;
    _tasks_in_work = 0;
    _tasks_for_check = 0;
    ui->marksTreeWidget->clear();
    QString facility, floor;
    switch( area->getType() )
    {
    case BaseArea::AT_LOCATION:{
        LocationPtr locationPtr = BaseArea::convert< Location >( area );
        if(locationPtr)
        {
            loadTasks("", "", locationPtr);

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
    case BaseArea::AT_LOCATION:{
        LocationPtr locationPtr = BaseArea::convert< Location >( area );
        if( ! locationPtr) return;

        MarkPtrs marks_of_location = locationPtr->getMarks();
        for( MarkPtr mark: marks_of_location )
            if(markInArchive(mark) == false)
                loadTasks("", "", "", mark);
        }break;

    case BaseArea::AT_FACILITY:{
        FacilityPtr facilityPtr = BaseArea::convert< Facility >( area );
        if( ! facilityPtr) return;

        MarkPtrs marks_of_facility = facilityPtr->getMarks();
        for( MarkPtr mark: marks_of_facility )
            if(markInArchive(mark) == false)
                loadTasks(facilityName, "", "", mark);

        FloorPtrs floors = facilityPtr->getChilds();
        for( FloorPtr floorPtr: floors )
            loadTasks(facilityName, floorPtr->getName(), floorPtr);
    }break;

    case BaseArea::AT_FLOOR:{
        FloorPtr floorPtr = BaseArea::convert< Floor >( area );
        if( ! floorPtr) return;

        MarkPtrs marks_of_floor = floorPtr->getMarks();
        for( MarkPtr mark: marks_of_floor )
            if(markInArchive(mark) == false)
                loadTasks(facilityName, floorName, "", mark);

        auto rooms = floorPtr->getChilds();
        for( BaseAreaPtr room_ptr: rooms )
        {
            RoomPtr room = BaseArea::convert< Room >( room_ptr );
            if(room)
                loadTasks(facilityName, floorName, room);
        }
    }break;

    case BaseArea::AT_ROOM:{
        RoomPtr room = BaseArea::convert< Room >( area );
        if(room)
        {
            MarkPtrs marks_of_room = room->getMarks();
            for( MarkPtr mark: marks_of_room )
                if(markInArchive(mark) == false)
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
    item->setData( 0, Qt::UserRole, (qulonglong)(ptr->getId()) );
}

void InfoForm::slotLoadImage()
{
    QStringList list = QFileDialog::getOpenFileNames(this);
    foreach (QString name, list)
    {
        QPixmap pm(name);
        if(pm.isNull())
            continue;

        int rotate(0);
        QImageReader imageReader(name);
        switch(imageReader.transformation())
        {
        case QImageIOHandler::TransformationRotate90 :{
            rotate = 90;
        }break;
        case QImageIOHandler::TransformationRotate180 :{
            rotate = 180;
        }break;
        case QImageIOHandler::TransformationRotate270 :{
            rotate = 270;
        }break;
        }

        if(rotate != 0)
            pm = pm.transformed(QTransform().rotate(rotate));

        _pixmaps.append(pm);
        _listWidget->addItem(pm);
    }
}

void InfoForm::slotApply()
{
    BaseAreaPtr area = RegionBizManager::instance()->getBaseArea(_id);
    if( ! area ) return;

    RoomPtr room = BaseArea::convert< Room >( area );
    if(room)
    {
        bool needCommit(false);

        BaseMetadataPtr areaPtr = room->getMetadata("area");
        if(areaPtr)
        {
            double square = ui->square->value();
            QString squareStr = QString::number(square, 'f', 1);
            if(squareStr != ui->square->property("area").toString())
            {
                room->addMetadata("double", "area", square);
                needCommit = true;
            }
        }

        BaseMetadataPtr rentorPtr = room->getMetadata("rentor");
        if(rentorPtr)
        {
            QString rentorStr = ui->renter->text();
            if(rentorStr != ui->renter->property("rentor").toString())
            {
                room->addMetadata("string", "rentor", rentorStr);
                needCommit = true;
            }
        }

        BaseMetadataPtr statusPtr = room->getMetadata("status");
        if(statusPtr)
        {
            QString statusStr = ui->status->currentText();
            if(statusStr != ui->status->property("status").toString())
            {
                room->addMetadata("string", "status", statusStr);
                needCommit = true;
            }
        }

        BaseMetadataPtr commentPtr = room->getMetadata("comment");
        if(commentPtr)
        {
            QString commentStr = ui->description->toPlainText();
            if(commentStr != ui->description->property("comment").toString())
            {
                room->addMetadata("string", "comment", commentStr);
                needCommit = true;
            }
        }

        if(needCommit)
            room->commit();
    }

    if(_pixmaps.isEmpty() == false)
    {
        QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
        if(regionBizInitJson_Path.isValid())
        {
            QString destPath = regionBizInitJson_Path.toString();
            QDir dir(destPath);
            dir.mkdir(QString::number(_id));
            destPath = destPath + QDir::separator() + QString::number(_id) + QDir::separator();
            int N(0);
            foreach(QPixmap pm, _pixmaps)
                pm.save(destPath + QString::number(QDateTime::currentMSecsSinceEpoch()) + QString("_") + QString::number(++N) + ".tiff");
        }
    }

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
        name = area->getDescription();
        break;
    case BaseArea::AT_REGION:
    case BaseArea::AT_FLOOR:
    case BaseArea::AT_ROOM:
        name = area->getName();
        break;
    }

    if( BaseArea::AT_REGION == area->getType() )
        return name;

    QString that_name = recursiveGetName( area->getParent() ) + " / " + name;
    return  that_name;
}

void InfoForm::slotHeaderSectionClicked(int index)
{
    ui->marksTreeWidget->sortByColumn(index);
}

void InfoForm::slotDoubleClickOnMark(QTreeWidgetItem *item, int)
{
    if(item)
    {
        quint64 id = item->data(0, Qt::UserRole).toULongLong();
        emit signalShowMarkInfoWidget(id);

        BaseAreaPtr area = RegionBizManager::instance()->getBaseArea(_id);
        if(area)
            reloadTasks(area);
    }
}

bool InfoForm::markInArchive(regionbiz::MarkPtr markPtr)
{
    bool _markInArchive(false);
    if(markPtr)
    {
        BaseMetadataPtr status = markPtr->getMetadata("status");
        if(status)
        {
            QString statusStr = status->getValueAsString();
            if(statusStr == QString::fromUtf8("в архиве"))
                _markInArchive = true;
        }
    }
    return _markInArchive;
}








