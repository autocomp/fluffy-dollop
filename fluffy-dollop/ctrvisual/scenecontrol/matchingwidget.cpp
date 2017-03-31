#include "matchingwidget.h"
#include "transformingstate.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QLabel>
#include <QFrame>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <ctrcore/visual/stateinterface.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/rasterdataprovider.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>

//#include "providersaver.h"

using namespace visualize_system;

MatchingWidget::MatchingWidget(uint rasterId, uint visualizerId, const QPolygonF &polygonInSceneCoords)
    : _rasterId(rasterId)
    , _visualizerId(visualizerId)
//    , _savedGeoData(0)
    , _matchingChanged(false)
{
    _currentStackData.vertexInSceneCoords = polygonInSceneCoords;

    QHBoxLayout * _vbLayout = new QHBoxLayout(this);

    QLabel * lb = new QLabel(tr("Transparency").append(" : 0%"), this);
    _vbLayout->addWidget(lb);

    _slider = new QSlider(Qt::Horizontal, this);
    _slider->setFixedWidth(100);
    _slider->setMinimum(0);
    _slider->setMaximum(99);
    _slider->setValue(0);
    _slider->setSingleStep(25);
    _slider->setToolTip(tr("Set image transparency"));
    connect(_slider, SIGNAL(valueChanged(int)), this, SLOT(slotSetTransparency(int)));
    _vbLayout->addWidget(_slider);

    lb = new QLabel(QString::fromUtf8("100%"), this);
    _vbLayout->addWidget(lb);

    QFrame * fr = new QFrame(this);
    fr->setFrameShape(QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken );
    _vbLayout->addWidget(fr);

    _tbTransform = new QToolButton(this);
    connect(_tbTransform, SIGNAL(toggled(bool)), this, SLOT(slotTransform(bool)));
    _tbTransform->setIcon(QIcon(":/img/ctrvisual_imgsize.png"));
    _tbTransform->setToolTip(tr("Transform image manually"));
    _tbTransform->setCheckable(true);
    _tbTransform->setFixedSize(32,32);
    _tbTransform->setIconSize(QSize(32,32));
    _tbTransform->setAutoRaise(true);
    _vbLayout->addWidget(_tbTransform);

    fr = new QFrame(this);
    fr->setFrameShape(QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken );
    _vbLayout->addWidget(fr);

    _tbSetPoint = new QToolButton(this);
    connect(_tbSetPoint, SIGNAL(clicked(bool)), this, SLOT(slotSetPoint(bool)));
    _tbSetPoint->setIcon(QIcon(":/img/ctrvisual_dot.png"));
    _tbSetPoint->setToolTip(tr("Rematching image"));
    _tbSetPoint->setCheckable(true);
    _tbSetPoint->setFixedSize(32,32);
    _tbSetPoint->setIconSize(QSize(32,32));
    _tbSetPoint->setAutoRaise(true);
    _vbLayout->addWidget(_tbSetPoint);

    _tbDeletPoint = new QToolButton(this);
    connect(_tbDeletPoint, SIGNAL(clicked()), this, SLOT(slotDeletPoint()));
    _tbDeletPoint->setIcon(QIcon(":/img/ctrvisual_close_or_delete.png"));
    _tbDeletPoint->setToolTip(tr("Delete current point"));
    _tbDeletPoint->setFixedSize(32,32);
    _tbDeletPoint->setIconSize(QSize(32,32));
    _tbDeletPoint->setAutoRaise(true);
    _tbDeletPoint->setEnabled(false);
    _vbLayout->addWidget(_tbDeletPoint);

    _tbApply = new QToolButton(this);
    connect(_tbApply, SIGNAL(clicked()), this, SLOT(slotApply()));
//    _tbApply->setIcon(QIcon(":/img/ctrvisual_saved.png"));
    _tbApply->setToolTip(QString::fromUtf8("Перепривязать по установленным точкам"));
    _tbApply->setFixedSize(32,32);
    _tbApply->setIconSize(QSize(32,32));
    _tbApply->setAutoRaise(true);
    _tbApply->setEnabled(false);
    _tbApply->setVisible(false);
    _vbLayout->addWidget(_tbApply);

    fr = new QFrame(this);
    fr->setFrameShape(QFrame::VLine);
    fr->setFrameShadow(QFrame::Sunken );
    _vbLayout->addWidget(fr);

    _tbSave = new QToolButton(this);
    connect(_tbSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    _tbSave->setIcon(QIcon(":/img/110_icons_32_ok.png"));
    _tbSave->setToolTip(tr("Apply transformation"));
    _tbSave->setFixedSize(32,32);
    _tbSave->setIconSize(QSize(32,32));
    _tbSave->setAutoRaise(true);
    _tbSave->setEnabled(false);
    _vbLayout->addWidget(_tbSave);

    _tbUndo = new QToolButton(this);
    connect(_tbUndo, SIGNAL(clicked()), this, SLOT(slotUndo()));
    _tbUndo->setIcon(QIcon(":/img/ctrvisual_back.png"));
    _tbUndo->setToolTip(tr("Cancel transformation"));
    _tbUndo->setFixedSize(32,32);
    _tbUndo->setIconSize(QSize(32,32));
    _tbUndo->setAutoRaise(true);
    _tbUndo->setEnabled(false);
    _vbLayout->addWidget(_tbUndo);

    QString fileName;
    QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(_rasterId).dynamicCast<data_system::RasterDataProvider>();
    if(rdp)
    {
        QFileInfo fi(rdp->getProviderUrl().toLocalFile());
        fileName = fi.fileName();
    }

    update();

    quint64 visualizerWindowId(0);
    visualize_system::ViewInterface * interface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    if(interface)
        visualizerWindowId = interface->getVisualizerWindowId();

    _iface = new EmbIFaceNotifier(this);
    QString tag = QString("MatchingWidget");
    quint64 widgetId = ewApp()->restoreWidget(tag, _iface);
    if(0 == widgetId)
    {
        ew::EmbeddedWidgetStruct struc;
        ew::EmbeddedHeaderStruct headStr;
        headStr.hasCloseButton = true;
        headStr.headerPixmap = ":/img/072_icons_32_newcords.png";
        struc.header = headStr;
        struc.iface = _iface;
        struc.widgetTag = tag;
        struc.minSize = QSize(500,12);
        struc.topOnHint = true;
        ewApp()->createWidget(struc, visualizerWindowId);
    }
    ewApp()->setWidgetTitle(_iface->id(), tr("Specify image matching").append(" : ") + fileName);
    ewApp()->setVisible(_iface->id(), true);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(widgetClose()));

//    QVariant result(QString("MatchingWidget"));
//    CommonMessageNotifier::send( (uint)visualize_system::BusTags::DisableComponent, result, QString("visualize_system"));
}

MatchingWidget::~MatchingWidget()
{
    slotSetTransparency(0);

    if(_tbTransform->isChecked())
    {
        slotTransform(false);
    }
    if(_tbSetPoint->isChecked())
    {
        slotSetPoint(false);
    }
    QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(_rasterId).dynamicCast<data_system::RasterDataProvider>();
    if(rdp)
    {
        if(_stack.size() > 0)
        {
            if( QMessageBox::question( this, tr("Attention"),
                                       tr("Are you really want to apply current matching values?"),
                                       QMessageBox::Apply, QMessageBox::Cancel) == QMessageBox::Apply)
            {
                _matchingChanged = true;

//                if(_savedGeoData)
//                    delete _savedGeoData;
//                _savedGeoData = rdp->geoImageObjectPtr()->get_geo_data()->clone();
            }
        }

        if(_matchingChanged)
        {
//            if(_savedGeoData)
//            {
//                rdp->setMatchingData(_savedGeoData, false); // устанавливаем сохранённую.
//                delete _savedGeoData;
//                _savedGeoData = 0;
//            }

            // отправить сигнал об изменении провайдера "_rasterId" в виджет-список слоёв.
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::RasterMatchingChanged, QVariant((quint64) _rasterId), QString("visualize_system"));
        }
        else
        {
            rdp->undo();
        }
    }

    ewApp()->removeWidget(_iface->id());
    delete _iface;
    _iface = 0;

    if(_transformingState)
    {
        _transformingState->emit_closeState();
        _transformingState.clear();
    }
    if(_matchingState)
    {
        _matchingState->emit_closeState();
        _matchingState.clear();
    }

//    QVariant result(QString("MatchingWidget"));
//    CommonMessageNotifier::send( (uint)visualize_system::BusTags::EnableComponent, result, QString("visualize_system"));
}

uint MatchingWidget::getMatchingProviderId()
{
    return _rasterId;
}

quint64 MatchingWidget::getVisualizerWindowId()
{
    if(_iface)
        return _iface->id();
    else
        return 0;
}

void MatchingWidget::slotSetTransparency(int value)
{
    if(_transformingState)
        _transformingState->setTrancparency(value);

    visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(_visualizerId);
    if(dataInterface)
        dataInterface->setProviderViewProperty(_rasterId, QString("transparency"), value);

    /*
    if(_transformingState)
        _transformingState->setTrancparency(value);

    QUrl url(_rasterId);
    data_system::RasterDataProvider* rdp = (data_system::RasterDataProvider*)data_system::DataProviderFactory::instance()->getProvider(url);
    if(rdp != NULL)
    {
        contur::common::ProviderDelegate delegate(rdp);
        delegate.setProperty(QString("transparency"), value);
    }
    */
}

void MatchingWidget::slotTransform(bool on_off)
{
    if(on_off)
    {
        _tbSave->setProperty("isChanged", _tbSave->isEnabled());
        _tbSave->setEnabled(false);

        QPixmap pixmap;
        int zLevel;
        QPolygonF vertexInSceneCoords;
        emit signal_getRasterParams(_rasterId, pixmap, zLevel, vertexInSceneCoords);
        if(pixmap.isNull() == false)
        {
            emit signal_setVisibleForRaster(0, _rasterId);

            _transformingState = QSharedPointer<TransformingState>(new TransformingState(_rasterId, pixmap, zLevel, vertexInSceneCoords, _slider->value()));
            connect(_transformingState.data(), SIGNAL(signalTransformFinished()), this, SLOT(slot_transformFinished()));
            visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(_visualizerId);
            if(stateInterface)
                stateInterface->setVisualizerState(_transformingState);

            _tbSetPoint->setEnabled(false);
            _tbDeletPoint->setEnabled(false);
            _tbApply->setEnabled(false);
            _tbUndo->setEnabled(false);
        }
        else
        {
            // отщелкнуть кнопку ?!
        }
    }
    else
    {
        if(_transformingState)
        {
            if(_transformingState->isChanged())
            {
                _stack.push(_currentStackData);
                _tbSave->setProperty("isChanged", true);

                _currentStackData.saver = (qulonglong)_transformingState.data();
                //_currentStackData.polinomialGeoData.clear();
                _currentStackData.matchingPoints.clear();
                _currentStackData.vertexInSceneCoords = _transformingState->getCurrentVertex();

                emit signal_applyTransformation(_rasterId, _currentStackData.vertexInSceneCoords);
            }

            _transformingState->emit_closeState();
            _transformingState.clear();
        }

        emit signal_setVisibleForRaster(_rasterId, 0);


        _tbSetPoint->setEnabled(true);
        _tbDeletPoint->setEnabled(false);
        _tbApply->setEnabled(false);
        _tbUndo->setEnabled(_stack.size() > 0);

        QVariant isChanged = _tbSave->property("isChanged");
        if(isChanged.isValid() && isChanged.toBool() == true)
            _tbSave->setEnabled(true);
        else
            _tbSave->setEnabled(false);
        _tbSave->setProperty("isChanged", false);
    }
}

void MatchingWidget::slotSetPoint(bool on_off)
{
    if(on_off)
    {
        _tbSave->setProperty("isChanged", _tbSave->isEnabled());
        _tbSave->setEnabled(false);

        QPolygonF polygonInSceneCoords;
        emit signal_getPolygonInSceneCoords(_rasterId, polygonInSceneCoords);
        if(_currentStackData.matchingPoints.isEmpty() == false)
            _matchingState = QSharedPointer<MatchingState>(new MatchingState(_rasterId, polygonInSceneCoords, _currentStackData.matchingPoints));
        else
            _matchingState = QSharedPointer<MatchingState>(new MatchingState(_rasterId, polygonInSceneCoords));

        visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(_visualizerId);
        if(stateInterface)
            stateInterface->setVisualizerState(_matchingState);

        connect(_matchingState.data(), SIGNAL(signalSetTransparancy(int)), this, SLOT(slot_setTransparancy(int)));
        connect(_matchingState.data(), SIGNAL(signalMatchingFinished()), this, SLOT(slot_matchingFinished()));
        connect(_matchingState.data(), SIGNAL(signalEnabledApplyButton(bool)), this, SLOT(slot_enabledApplyButton(bool)));
        connect(_matchingState.data(), SIGNAL(signalEnabledDeleteButton(bool)), this, SLOT(slot_enabledDeleteButton(bool)));

        _tbTransform->setEnabled(false);
        _tbDeletPoint->setEnabled(false);
        _tbApply->setEnabled(false);
        _tbUndo->setEnabled(false);
    }
    else
    {
        if(_matchingState)
        {
            if(_tbApply->isEnabled())
                slotApply();

            _matchingState->emit_closeState();
            _matchingState.clear();
        }

        _tbTransform->setEnabled(true);
        _tbDeletPoint->setEnabled(false);
        _tbApply->setEnabled(false);
        _tbUndo->setEnabled(_stack.size() > 0);

        QVariant isChanged = _tbSave->property("isChanged");
        if(isChanged.isValid() && isChanged.toBool() == true)
            _tbSave->setEnabled(true);
        else
            _tbSave->setEnabled(false);
        _tbSave->setProperty("isChanged", false);
    }
}

void MatchingWidget::slotApply()
{
    if(_matchingState)
    {
        QList<MatchingState::MatchingPoint> matchingPoints = _matchingState->getMatchingPoints();
        if(matchingPoints.size() >= 3)
        {
            _matchingState->rasterMatched();
            //dpf::geo::PolinomialGeoData data;
            //emit signal_applyMatching(_rasterId, matchingPoints, data);

            qulonglong saver = (qulonglong)_matchingState.data();
            if(_currentStackData.saver != saver)
            {
                _stack.push(_currentStackData);
                _currentStackData.vertexInSceneCoords.clear();
                _currentStackData.saver = saver;
            }
            //_currentStackData.polinomialGeoData = data;
            _currentStackData.matchingPoints = _matchingState->getMatchingPoints();

            QPolygonF polygonInSceneCoords;
            emit signal_getPolygonInSceneCoords(_rasterId, polygonInSceneCoords);
            _matchingState->setRasterScenePolygon(polygonInSceneCoords);

            _tbSave->setProperty("isChanged", true);
        }
    }
}

void MatchingWidget::slotDeletPoint()
{
    if(_matchingState)
        _matchingState->deleteSelectedPoint();
}

void MatchingWidget::slotSave()
{
    QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(_rasterId).dynamicCast<data_system::RasterDataProvider>();
    if(rdp)
    {
//        QVariant var = rdp->getMetaData("ProviderDontMatched");
//        if(var.isValid() && var.toBool() == true)
//            rdp->setMetaData("ProviderDontMatched", false);

//        if(_savedGeoData)
//            delete _savedGeoData;
//        _savedGeoData = rdp->geoImageObjectPtr()->get_geo_data()->clone();

        _currentStackData.saver = 0;
        _stack.clear();
        _tbSave->setEnabled(false);
        _tbUndo->setEnabled(false);

        _matchingChanged = true;
    }
}

void MatchingWidget::slotUndo()
{
    if(_stack.size() > 0)
    {
        _currentStackData = _stack.pop();

        if(_currentStackData.vertexInSceneCoords.isEmpty() == false)
        {
            emit signal_applyTransformation(_rasterId, _currentStackData.vertexInSceneCoords);
        }
//        else if(_currentStackData.polinomialGeoData.is_null() == false)
//        {
//            emit signal_applyMatching(_rasterId, QList<MatchingState::MatchingPoint>(), _currentStackData.polinomialGeoData);
//        }

        _tbUndo->setEnabled(_stack.size() > 0);
    }
    else
        _tbSave->setEnabled(false);
}

//void MatchingWidget::slotClose()
//{
//    close();
//}

void MatchingWidget::slot_transformFinished()
{
    _tbTransform->setChecked(false);
}

void MatchingWidget::slot_matchingFinished()
{
    _tbSetPoint->setChecked(false);
}

void MatchingWidget::slot_setTransparancy(int val)
{
    _slider->setValue(val);
}

void MatchingWidget::slot_enabledApplyButton(bool on_off)
{
    _tbApply->setEnabled(on_off);
}

void MatchingWidget::slot_enabledDeleteButton(bool on_off)
{
    _tbDeletPoint->setEnabled(on_off);
}

void MatchingWidget::widgetClose()
{
    QVariant result(QString(""));
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MatchingProviderWidgetClosed, result, QString("visualize_system"));
}






