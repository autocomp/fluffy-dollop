#include "viragraphicsplugin.h"
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/abstractscenewidget.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/abstractdataprovider.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <regionbiz/rb_manager.h>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QStackedWidget>
#include <QDebug>

using namespace regionbiz;

ViraGraphicsPlugin::ViraGraphicsPlugin()
{
}

ViraGraphicsPlugin::~ViraGraphicsPlugin()
{
}

void ViraGraphicsPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

    visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    _stackedWidget = dynamic_cast<QStackedWidget*>(viewInterface->stackedWidget());

    QTimer::singleShot(1000, this, SLOT(launchWorkState()));
}
void ViraGraphicsPlugin::launchWorkState()
{
    _workState = QSharedPointer<WorkState>(new WorkState());
    connect(_workState.data(), SIGNAL(showFacility(qulonglong)), this, SLOT(showFacility(qulonglong)));
    connect(_workState.data(), SIGNAL(switchOnMap()), this, SLOT(switchOnMap()));
    connect(_workState.data(), SIGNAL(switchOnEditor()), this, SLOT(switchOnEditor()));
    connect(_workState.data(), SIGNAL(setMarkOnMap(qulonglong)), this, SLOT(setMarkOnMap(qulonglong)));
    connect(_workState.data(), SIGNAL(editAreaGeometry(bool)), this, SLOT(editAreaGeometry(bool)));
    visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_workState);

    _pixelVisualizerId = visualize_system::VisualizerManager::instance()->addVisualizer(visualize_system::VisualizerPixel);
     visualize_system::ViewInterface * pixelVisualizerViewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(_pixelVisualizerId);

    _pixelWorkState = QSharedPointer<PixelWorkState>(new PixelWorkState(_pixelVisualizerId));
    connect(_pixelWorkState.data(), SIGNAL(setMarkOnPlan(qulonglong, QPolygonF)), this, SLOT(setMarkOnPlan(qulonglong, QPolygonF)));
    connect(_pixelWorkState.data(), SIGNAL(switchOnEditor()), this, SLOT(switchOnEditor()));
    connect(_pixelWorkState.data(), SIGNAL(editAreaGeometry(bool)), this, SLOT(editAreaGeometry(bool)));
    visualize_system::VisualizerManager::instance()->getStateInterface(_pixelVisualizerId)->setVisualizerState(_pixelWorkState);

    int visualizerIndex = _stackedWidget->currentIndex();
    _stackedWidget->setProperty("visualizerIndex", visualizerIndex);
    int pdfEditorFormIndex = _stackedWidget->addWidget(pixelVisualizerViewInterface->widget());
    _stackedWidget->setProperty("pdfEditorFormIndex", pdfEditorFormIndex);
    _stackedWidget->setCurrentIndex(visualizerIndex);

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnCurrentChange(this, SLOT(currentItemsChanged(uint64_t,uint64_t)));
}

void ViraGraphicsPlugin::showFacility(qulonglong id)
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("pdfEditorFormIndex").toInt());
}

void ViraGraphicsPlugin::switchOnMap()
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("visualizerIndex").toInt());
}

void ViraGraphicsPlugin::switchOnEditor()
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("pdfEditorFormIndex").toInt());
}

void ViraGraphicsPlugin::setMarkOnMap(qulonglong markType)
{
    if(markType > 0)
    {
        _stackedWidget->setCurrentIndex(_stackedWidget->property("visualizerIndex").toInt());

        switch(markType)
        {
        case 1 :{ // defect
            _choiceAreaState = QSharedPointer<ChoiceAreaState>(new ChoiceAreaState(ChoiceAreaState::POINT_OR_POLYGON, QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0)));
            visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_choiceAreaState);
            connect(_choiceAreaState.data(), SIGNAL(signalAreaChoiced(QPolygonF)), this, SLOT(defectStateChoiced(QPolygonF)));
            connect(_choiceAreaState.data(), SIGNAL(signalAbort()), this, SLOT(markCreatingAbort()));
        }break;
        case 2 :{ // foto
            _setImageState = QSharedPointer<SetImageState>(new SetImageState());
            visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_setImageState);
            connect(_setImageState.data(), SIGNAL(signalCreated(QPointF,double)), this, SLOT(fotoCreared(QPointF,double)));
            connect(_setImageState.data(), SIGNAL(signalAbort()), this, SLOT(markCreatingAbort()));
        }break;
        case 3 :{ // foto360
            _choiceAreaState = QSharedPointer<ChoiceAreaState>(new ChoiceAreaState(ChoiceAreaState::POINT, QCursor(QPixmap(":/img/cursor_foto360.png"), 0, 0)));
            visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_choiceAreaState);
            connect(_choiceAreaState.data(), SIGNAL(signalAreaChoiced(QPolygonF)), this, SLOT(foto360Creared(QPolygonF)));
            connect(_choiceAreaState.data(), SIGNAL(signalAbort()), this, SLOT(markCreatingAbort()));
        }break;
        }
    }
    else
    {
        if(_choiceAreaState)
        {
            _choiceAreaState->emit_closeState();
            _choiceAreaState.clear();
        }
        if(_setImageState)
        {
            _setImageState->emit_closeState();
            _setImageState.clear();
        }
    }
}

void ViraGraphicsPlugin::setMarkOnPlan(qulonglong markType, QPolygonF area)
{
    if(markType > 0)
    {
        _stackedWidget->setCurrentIndex(_stackedWidget->property("pdfEditorFormIndex").toInt());

        switch(markType)
        {
        case 1 :{ // defect
            _choiceAreaState = QSharedPointer<ChoiceAreaState>(new ChoiceAreaState(ChoiceAreaState::POINT_OR_POLYGON, QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0)));
            _choiceAreaState->setBoundingArea(area);
            visualize_system::VisualizerManager::instance()->getStateInterface(_pixelVisualizerId)->setVisualizerState(_choiceAreaState);
            connect(_choiceAreaState.data(), SIGNAL(signalAreaChoiced(QPolygonF)), this, SLOT(defectStateChoiced(QPolygonF)));
            connect(_choiceAreaState.data(), SIGNAL(signalAbort()), this, SLOT(markCreatingAbort()));
        }break;
        case 2 :{ // foto
            _setImageState = QSharedPointer<SetImageState>(new SetImageState());
            _setImageState->setBoundingArea(area);
            visualize_system::VisualizerManager::instance()->getStateInterface(_pixelVisualizerId)->setVisualizerState(_setImageState);
            connect(_setImageState.data(), SIGNAL(signalCreated(QPointF,double)), this, SLOT(fotoCreared(QPointF,double)));
            connect(_setImageState.data(), SIGNAL(signalAbort()), this, SLOT(markCreatingAbort()));
        }break;
        case 3 :{ // foto360
            _choiceAreaState = QSharedPointer<ChoiceAreaState>(new ChoiceAreaState(ChoiceAreaState::POINT, QCursor(QPixmap(":/img/cursor_foto360.png"), 0, 0)));
            _choiceAreaState->setBoundingArea(area);
            visualize_system::VisualizerManager::instance()->getStateInterface(_pixelVisualizerId)->setVisualizerState(_choiceAreaState);
            connect(_choiceAreaState.data(), SIGNAL(signalAreaChoiced(QPolygonF)), this, SLOT(foto360Creared(QPolygonF)));
            connect(_choiceAreaState.data(), SIGNAL(signalAbort()), this, SLOT(markCreatingAbort()));
        }break;
        }
    }
    else
    {
        if(_choiceAreaState)
        {
            _choiceAreaState->emit_closeState();
            _choiceAreaState.clear();
        }
        if(_setImageState)
        {
            _setImageState->emit_closeState();
            _setImageState.clear();
        }
    }
}

void ViraGraphicsPlugin::defectStateChoiced(QPolygonF pol)
{
    // create or change mark !
    if(_choiceAreaState)
    {
        _choiceAreaState->emit_closeState();
        _choiceAreaState.clear();
    }
    QList<QVariant>list;

    QVariant type(1);
    list.append(type);

    QVariant polygon;
    polygon.setValue(pol);
    list.append(polygon);

    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

void ViraGraphicsPlugin::fotoCreared(QPointF pos, double direction)
{
    if(_setImageState)
    {
        _setImageState->emit_closeState();
        _setImageState.clear();
    }

    QList<QVariant>list;
    QVariant type(2);
    list.append(type);
    list.append(pos);
    list.append(direction);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

void ViraGraphicsPlugin::markCreatingAbort()
{
    if(_choiceAreaState)
    {
        _choiceAreaState->emit_closeState();
        _choiceAreaState.clear();
    }
    if(_setImageState)
    {
        _setImageState->emit_closeState();
        _setImageState.clear();
    }

    QList<QVariant>list;
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

void ViraGraphicsPlugin::foto360Creared(QPolygonF pol)
{
    if(_setImageState)
    {
        _setImageState->emit_closeState();
        _setImageState.clear();
    }

    QList<QVariant>list;
    if(pol.isEmpty() == false)
    {
        QVariant type(3);
        list.append(type);
        list.append(pol.first());
    }
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

void ViraGraphicsPlugin::editAreaGeometry(bool on_off)
{
    if(on_off)
    {
        if(sender() == _workState.data())
            _editedVisualizerId = getVisualizerId();
        else if(sender() == _pixelWorkState.data())
            _editedVisualizerId = _pixelVisualizerId;

        _choiceAreaState = QSharedPointer<ChoiceAreaState>(new ChoiceAreaState(ChoiceAreaState::POLYGON, QCursor(QPixmap(":/img/cursor_polygon.png"), 0, 0)));
        visualize_system::VisualizerManager::instance()->getStateInterface(_editedVisualizerId)->setVisualizerState(_choiceAreaState);
        connect(_choiceAreaState.data(), SIGNAL(signalAreaChoiced(QPolygonF)), this, SLOT(areaGeometryEdited(QPolygonF)));
        connect(_choiceAreaState.data(), SIGNAL(signalAbort()), this, SLOT(areaGeometryEditingAbort()));
    }
    else
    {
        if(_choiceAreaState)
        {
            _choiceAreaState->emit_closeState();
            _choiceAreaState.clear();
        }
        _editedVisualizerId = 0;
    }
}

void ViraGraphicsPlugin::areaGeometryEdited(QPolygonF pol)
{
    if(_choiceAreaState)
    {
        _choiceAreaState->emit_closeState();
        _choiceAreaState.clear();

        if(_editedVisualizerId == getVisualizerId())
            _workState->areaGeometryEdited(pol);
        else if(_editedVisualizerId == _pixelVisualizerId)
            _pixelWorkState->areaGeometryEdited(pol);
    }
}

void ViraGraphicsPlugin::areaGeometryEditingAbort()
{
    if(_choiceAreaState)
    {
        _choiceAreaState->emit_closeState();
        _choiceAreaState.clear();

        if(_editedVisualizerId == getVisualizerId())
            _workState->areaGeometryEdited(QPolygonF());
        else if(_editedVisualizerId == _pixelVisualizerId)
            _pixelWorkState->areaGeometryEdited(QPolygonF());
    }
}

void ViraGraphicsPlugin::currentItemsChanged(uint64_t, uint64_t curr_id)
{
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
    if(ptr)
        switch(ptr->getType())
        {
        case BaseArea::AT_REGION :
        case BaseArea::AT_LOCATION :
        case BaseArea::AT_FACILITY :
           if(_stackedWidget->currentIndex() != _stackedWidget->property("visualizerIndex").toInt())
               switchOnMap();
        break;
        case BaseArea::AT_FLOOR :
        case BaseArea::AT_ROOM :
            if(_stackedWidget->currentIndex() != _stackedWidget->property("pdfEditorFormIndex").toInt())
                switchOnEditor();
        break;
        }
}

QList<InitPluginData> ViraGraphicsPlugin::getInitPluginData()
{
    InitPluginData initDataSave;
    initDataSave.buttonName = QString("ViraGraphicsPlugin");
    initDataSave.translateButtonName = QString::fromUtf8("Switch base cover");
    initDataSave.iconForButtonOn = QIcon(":/img/119_icons_32_gl.png");
    initDataSave.isCheckable = false;
    initDataSave.tooltip = QString::fromUtf8("Switch base cover");

    return QList<InitPluginData>() << initDataSave;
}

bool ViraGraphicsPlugin::isChecked(const QString&)
{
    return false;
}

void ViraGraphicsPlugin::checked(const QString &buttonName, bool on_off)
{
    if(buttonName == QString("ViraGraphicsPlugin"))
    {
        visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(getVisualizerId());
        QList<uint>providers = dataInterface->getProviders();
        if(providers.size() == 2)
        {
            dataInterface->setProviderViewProperty(providers.at(0), "visibility", ! _showFirstBaseCover);
            dataInterface->setProviderViewProperty(providers.at(1), "visibility", _showFirstBaseCover);
        }
        _showFirstBaseCover = ! _showFirstBaseCover;
    }
}



































