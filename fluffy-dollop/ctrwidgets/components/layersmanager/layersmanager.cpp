#include "layersmanager.h"
#include "layersmanagerform.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <regionbiz/rb_manager.h>
#include <ctrwidgets/components/slidewindow/slide_window.h>
#include <QPushButton>
#include <QTimer>
using namespace regionbiz;

LayersManager::LayersManager()
{
    QTimer::singleShot(1000, this, SLOT(init()));


}

LayersManager::~LayersManager()
{
    delete _layersManagerForm;
}

void LayersManager::init()
{
    SlideWindow* sw = SlideWindow::mainInstance();

    _layersButton = new QPushButton();
    _layersButton->setCheckable(true);
    _layersButton->setChecked(true);
    _layersButton->setMinimumSize( 40, 40 );
    _layersButton->setMaximumSize( 40, 40 );
    _layersButton->setIcon( QIcon( ":/img/019_icons_32_layers.png" ));
    _layersButton->setToolTip( QString::fromUtf8("Слои"));
    connect(_layersButton, SIGNAL(clicked(bool)), this, SLOT(setVisible(bool)));
    sw->addWidget( _layersButton );

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnCurrentChange(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));

    _layersManagerForm = new LayersManagerForm;

    _iface = new EmbIFaceNotifier(_layersManagerForm);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotCloseWindow()));
    QString tag = QString("ViraLayersManager_MainWindow");
    quint64 widgetId = ewApp()->restoreWidget(tag, _iface);
    if(0 == widgetId)
    {
        ew::EmbeddedWidgetStruct struc;
        ew::EmbeddedHeaderStruct headStr;
        headStr.hasCloseButton = true;
        headStr.hasMinMaxButton = false;
        headStr.hasCollapseButton = false;
        headStr.headerPixmap = ":/img/019_icons_32_layers.png";
        headStr.windowTitle = QString::fromUtf8("Слои");
        struc.header = headStr;
        struc.iface = _iface;
        struc.widgetTag = tag;
        struc.minSize = QSize(200,200);
        struc.topOnHint = true;
        struc.isModal = false;
        ewApp()->createWidget(struc);
    }
    reset();
}

void LayersManager::setVisible(bool on_off)
{
    if( !_iface )
        return;

    ewApp()->setVisible(_iface->id(), on_off);
}

void LayersManager::slotCloseWindow()
{
    _layersButton->blockSignals(true);
    _layersButton->setChecked(false);
    _layersButton->blockSignals(false);
}

void LayersManager::slotObjectSelectionChanged(uint64_t /*prev_id*/, uint64_t curr_id)
{
    BaseEntityPtr entity = RegionBizManager::instance()->getBaseEntity(curr_id);
    if( !entity )
    {
        _currentObject = 0;
        reset();
        return;
    }

    switch( entity->getEntityType() )
    {
    case BaseEntity::ET_MARK:
    {
        _currentObject = 0;
        reset();
    }return;
    case BaseEntity::ET_AREA:
    {
        BaseAreaPtr ptr = entity->convert< BaseArea >();
        if(! ptr)
        {
            _currentObject = 0;
            reset();
            return;
        }

        uint64_t currentObject = 0;
        switch( ptr->getType() )
        {
        case BaseArea::AT_REGION:
            _currentObject = 0;
            reset();
            return;
        case BaseArea::AT_LOCATION:
        case BaseArea::AT_FACILITY:
        case BaseArea::AT_FLOOR:
            currentObject = ptr->getId();
            break;
        case BaseArea::AT_ROOM:
            currentObject = ptr->getParentId();
            break;
        }
        if(currentObject == 0)
        {
            _currentObject = 0;
            reset();
            return;
        }
        if(currentObject == _currentObject)
            return;

        _currentObject = currentObject;
        ptr = RegionBizManager::instance()->getBaseArea(_currentObject);
        if(ptr)
            reload(ptr);
    }
    }
}

void LayersManager::reset()
{
    ewApp()->setWidgetTitle(_iface->id(), QString(""));
    _layersManagerForm->reset();
}

void LayersManager::reload(BaseAreaPtr ptr)
{
    ewApp()->setWidgetTitle(_iface->id(), ptr->getName());
    _layersManagerForm->reload(ptr);
}



















