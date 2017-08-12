#include "layerinstrumentalform.h"
#include "ui_layerinstrumentalform.h"
#include <QTimer>
#include <QPixmap>
#include <QDebug>
#include <QColorDialog>
#include "pixmaptransformstate.h"
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <libembeddedwidgets/embeddedapp.h>

using namespace pixmap_transform_state;

LayerInstrumentalForm::LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, QWidget *parent)
    : QWidget(parent)
    , _visualizerId(visualizerId)
    , ui(new Ui::LayerInstrumentalForm)
{
    ui->setupUi(this);

    connect(ui->changeColorRB, SIGNAL(clicked(bool)), this, SLOT(applyForImageOrArea(bool)));
    connect(ui->fillRB, SIGNAL(clicked(bool)), this, SLOT(applyForImageOrArea(bool)));

    ui->transform->setIcon(QIcon(":/img/icon_transform.png"));
    connect(ui->transform, SIGNAL(clicked(bool)), this, SLOT(setMode(bool)));
    ui->crop->setIcon(QIcon(":/img/icon_crop.png"));
    connect(ui->crop, SIGNAL(clicked(bool)), this, SLOT(setMode(bool)));
    ui->changeColor->setIcon(QIcon(":/img/icon_get_color.png"));
    connect(ui->changeColor, SIGNAL(clicked(bool)), this, SLOT(setMode(bool)));

    ui->save->setIcon(QIcon(":/img/icon_save.png"));
    connect(ui->save, SIGNAL(clicked()), this, SLOT(save()));

    ui->persentFrame->setVisible(false);
    connect(ui->persentSlider, SIGNAL(valueChanged(int)), this, SLOT(setOpacityValue(int)));

    ui->undoAction->setIcon(QIcon(":/img/icon_undo_act.png"));
    ui->undoAction->setEnabled(false);
    connect(ui->undoAction, SIGNAL(clicked()), this, SLOT(undoAction()));

    ui->apply->setIcon(QIcon(":/img/icon_apply.png"));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(apply()));

    ui->colorFrame->setVisible(false);
    ui->setColorOutFromImage->setIcon(QIcon(":/img/icon_get_color.png"));
    connect(ui->setColorOutFromImage, SIGNAL(clicked(bool)), this, SLOT(GetColorOnImage(bool)));

    ui->setColorInFromImage->setIcon(QIcon(":/img/icon_get_color.png"));
    connect(ui->setColorInFromImage, SIGNAL(clicked(bool)), this, SLOT(GetColorOnImage(bool)));

    ui->setArea->setIcon(QIcon(":/img/icon_polygon.png"));
    connect(ui->setArea, SIGNAL(clicked(bool)), this, SLOT(setArea(bool)));
    ui->setClearArea->setIcon(QIcon("://img/icon_delete.png"));
    ui->setClearArea->setEnabled(false);
    connect(ui->setClearArea, SIGNAL(clicked()), this, SLOT(clearArea()));

    QPixmap colorOutPixmap(32,32);
    _colorOut = Qt::white;
    colorOutPixmap.fill(_colorOut);
    ui->setColorOutFromDialog->setIcon(QIcon(colorOutPixmap));
    connect(ui->setColorOutFromDialog, SIGNAL(clicked()), this, SLOT(pressSetColor()));

    QPixmap colorInPixmap(32,32);
    _colorIn = Qt::black;
    colorInPixmap.fill(_colorIn);
    ui->setColorInFromDialog->setIcon(QIcon(colorInPixmap));
    connect(ui->setColorInFromDialog, SIGNAL(clicked()), this, SLOT(pressSetColor()));

    connect(ui->setTransparent, SIGNAL(clicked(bool)), this, SLOT(setColorMode(bool)));
    connect(ui->setColor, SIGNAL(clicked(bool)), this, SLOT(setColorMode(bool)));

    QPointF pos(0,0);
    double originalScale(-1);
    visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(_visualizerId);
    if(viewInterface->getVisualizerType() == visualize_system::Visualizer2D)
    {
        QRectF viewportSceneRect = viewInterface->getViewportSceneRect();
        double v_W = viewportSceneRect.width() / 5.;
        double v_H = viewportSceneRect.height() / 5.;
        viewportSceneRect = QRectF( viewportSceneRect.x() + v_W * 2.,
                                    viewportSceneRect.y() + v_H * 2.,
                                    v_W,
                                    v_H );

        QSize srcSize = pixmap.size();

        double w_inscribeByW = viewportSceneRect.width();
        double h_inscribeByW = viewportSceneRect.width() * (double)srcSize.height() / (double)srcSize.width();

        double w_inscribeByH = viewportSceneRect.height() * (double)srcSize.width() / (double)srcSize.height();
        double h_inscribeByH = viewportSceneRect.height();

        double X(0), Y(0), W(0), H(0);
        if(h_inscribeByW <= viewportSceneRect.height())
        {
            X = viewportSceneRect.x();
            Y = viewportSceneRect.y() + (viewportSceneRect.height() - h_inscribeByW)/2.;
            W = w_inscribeByW;
            H = h_inscribeByW;
            originalScale = W / srcSize.width();
        }
        else
        {
            X = viewportSceneRect.x() + (viewportSceneRect.width() - w_inscribeByH)/2.;
            Y = viewportSceneRect.y();
            W = w_inscribeByH;
            H = h_inscribeByH;
            originalScale = W / srcSize.width();
        }
        pos = QPointF(X,Y);
    }

    _pixmapTransformState = QSharedPointer<PixmapTransformState>(new PixmapTransformState(pixmap, pos, originalScale));
    visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(_visualizerId);
    connect(_pixmapTransformState.data(), SIGNAL(signalSendColor(QColor)), this, SLOT(setColorOnImage(QColor)));
    connect(_pixmapTransformState.data(), SIGNAL(signalAreaSetted()), this, SLOT(areaSetted()));
    connect(_pixmapTransformState.data(), SIGNAL(signalPixmapChanged()), this, SLOT(pixmapChanged()));
    if(stateInterface)
        stateInterface->setVisualizerState(_pixmapTransformState);

    QTimer::singleShot(5,this,SLOT(makeAdjustForm()));
}

LayerInstrumentalForm::~LayerInstrumentalForm()
{
    delete ui;

    if(_pixmapTransformState)
    {
        _pixmapTransformState->emit_closeState();
        _pixmapTransformState.clear();
    }
}

void LayerInstrumentalForm::setEmbeddedWidgetId(quint64 id)
{
    _embeddedWidgetId = id;
}

//QWidget *LayerInstrumentalForm::getWidget()
//{
//    return this;
//}

//void LayerInstrumentalForm::closed(bool *acceptFlag)
//{
//    if(acceptFlag)
//    {
//        // *acceptFlag = false;
//        emit signalClosed();
//    }
//}

void LayerInstrumentalForm::setMode(bool on_off)
{
    bool changeColorModeOff(false);
    if(on_off)
    {
        if(ui->transform == sender())
        {
            ui->apply->setEnabled(false);
            _pixmapTransformState->setMode(StateMode::TransformImage);
        }
        else
            ui->transform->setChecked(false);

        if(ui->crop == sender())
        {
            ui->apply->setEnabled(true);
            _pixmapTransformState->setMode(StateMode::CropImage);
        }
        else
            ui->crop->setChecked(false);

        if(ui->changeColor == sender())
        {
            ui->apply->setEnabled(true);
            _pixmapTransformState->setMode(StateMode::ScrollMap);
        }
        else
        {
            if(ui->changeColor->isChecked())
            {
                if(ui->crop != sender())
                    changeColorModeOff = true;
                ui->changeColor->setChecked(false);
            }
        }
    }
    else
    {
        if(ui->crop == sender())
            _pixmapTransformState->cropPixmap();

        _pixmapTransformState->setMode(StateMode::ScrollMap);
        changeColorModeOff = true;
    }

    if(changeColorModeOff)
    {
        _pixmapTransformState->clearAreaOnImage();
        ui->setColorInFromImage->setChecked(false);
        ui->setColorOutFromImage->setChecked(false);
        ui->setArea->setChecked(false);
        ui->setClearArea->setEnabled(false);

        changeColorRBdependElementsEnabled(true);
        ui->fillRB->setChecked(false);
        ui->fillRB->setEnabled(false);

        ui->setTransparent->setText(QString::fromUtf8("на прозрачный"));
        ui->setColor->setText(QString::fromUtf8("на цвет"));
    }

    ui->undoAction->setEnabled(_pixmapTransformState->changed());

    if(! ui->transform->isChecked() && ! ui->crop->isChecked() && ! ui->changeColor->isChecked())
    {
        ui->apply->setEnabled(_pixmapTransformState->changed());
        ui->save->setEnabled(_pixmapTransformState->changed());
    }

    ui->persentSlider->setValue(0);
    setOpacityValue(0);

    ui->persentFrame->setVisible(ui->transform->isChecked());

    ui->colorFrame->setVisible( ui->changeColor->isChecked() );

    _pixmapTransformState->setTransparentBackgroundForPixmapItem( ui->changeColor->isChecked() );

    QTimer::singleShot(50,this,SLOT(makeAdjustForm()));
}

void LayerInstrumentalForm::makeAdjustForm()
{
    QWidget * wgt = this;
    QSize wgtMinSize = wgt->minimumSize();
    QSize wgtMaxSize = wgt->maximumSize();
    wgt->setFixedWidth(wgt->size().width());
    adjustSize();
    wgt->resize(0,0);
    ewApp()->adjustWidgetSize(_embeddedWidgetId);
    wgt->setMinimumWidth(wgtMinSize.width());
    wgt->setMaximumWidth(wgtMaxSize.width());
}

void LayerInstrumentalForm::setOpacityValue(int precent)
{
    _pixmapTransformState->setTransparency(precent);
}

void LayerInstrumentalForm::apply()
{
    if(ui->transform->isChecked())
    {


    }
    else if(ui->crop->isChecked())
    {
        _pixmapTransformState->cropPixmap();
    }
    else if(ui->changeColor->isChecked())
    {
        if(ui->changeColorRB->isChecked())
        {
            if(ui->setColor->isChecked())
                _pixmapTransformState->changeImageColor(_colorIn, _colorOut, ui->sensSlider->value());
            else if(ui->setTransparent->isChecked())
                _pixmapTransformState->changeImageColor(QColor(), _colorOut, ui->sensSlider->value());
        }
        else if(ui->fillRB->isChecked())
        {
            if(ui->setColor->isChecked())
                _pixmapTransformState->changeImageColor(_colorIn, QColor());
            else if(ui->setTransparent->isChecked())
                _pixmapTransformState->changeImageColor(QColor(), QColor());
        }
    }
}

void LayerInstrumentalForm::GetColorOnImage(bool on_off)
{
    if(on_off)
    {
        if(sender() == ui->setColorOutFromImage)
        {
            _pixmapTransformState->setMode(StateMode::GetColorOnImage);
            if(ui->setColorInFromImage->isChecked())
                ui->setColorInFromImage->setChecked(false);
        }
        if(sender() == ui->setColorInFromImage)
        {
            _pixmapTransformState->setMode(StateMode::GetColorOnImage);
            if(ui->setColorOutFromImage->isChecked())
                ui->setColorOutFromImage->setChecked(false);
        }
        if(ui->setArea->isChecked())
        {
            _pixmapTransformState->clearAreaOnImage();
            ui->setArea->setChecked(false);
            ui->setClearArea->setEnabled(false);

            changeColorRBdependElementsEnabled(true);
            ui->fillRB->setChecked(false);
            ui->fillRB->setEnabled(false);

            ui->setTransparent->setText(QString::fromUtf8("на прозрачный"));
            ui->setColor->setText(QString::fromUtf8("на цвет"));
        }

        ui->apply->setEnabled(false);
        ui->undoAction->setEnabled(false);
    }
    else
    {
        _pixmapTransformState->setMode(StateMode::ScrollMap);

        bool changed(true);
        ui->apply->setEnabled(changed);
        ui->undoAction->setEnabled(_pixmapTransformState->changed());
    }
}

void LayerInstrumentalForm::setColorOnImage(QColor color)
{
    if(color.isValid())
    {
        QPixmap colorPixmap(32,32);
        if(ui->setColorInFromImage->isChecked())
        {
            _colorIn = color;
            color.setAlpha(255);
            colorPixmap.fill(color);
            ui->setColorInFromDialog->setIcon(QIcon(colorPixmap));
            ui->setColorInFromImage->setChecked(false);
        }
        else if(ui->setColorOutFromImage->isChecked())
        {
            _colorOut = color;
            color.setAlpha(255);
            colorPixmap.fill(color);
            ui->setColorOutFromDialog->setIcon(QIcon(colorPixmap));
            ui->setColorOutFromImage->setChecked(false);
        }
    }
    _pixmapTransformState->setMode(StateMode::ScrollMap);

    bool changed(true);
    ui->apply->setEnabled(changed);
    ui->undoAction->setEnabled(_pixmapTransformState->changed());
    ui->save->setEnabled(_pixmapTransformState->changed());
}

void LayerInstrumentalForm::setArea(bool on_off)
{
    if(on_off)
    {
        _pixmapTransformState->setMode(StateMode::GetAreaOnImage);
        ui->setClearArea->setEnabled(false);

        if(ui->setColorOutFromImage->isChecked())
           ui->setColorOutFromImage->setChecked(false);

        if(ui->setColorInFromImage->isChecked())
           ui->setColorInFromImage->setChecked(false);

        ui->apply->setEnabled(false);
        ui->undoAction->setEnabled(_pixmapTransformState->changed());
        ui->save->setEnabled(_pixmapTransformState->changed());
    }
    else
    {
        _pixmapTransformState->clearAreaOnImage();
        ui->setClearArea->setEnabled(false);
        _pixmapTransformState->setMode(StateMode::ScrollMap);

        changeColorRBdependElementsEnabled(true);
        ui->fillRB->setChecked(false);
        ui->fillRB->setEnabled(false);

        ui->setTransparent->setText(QString::fromUtf8("на прозрачный"));
        ui->setColor->setText(QString::fromUtf8("на цвет"));

        bool changed(true);
        ui->apply->setEnabled(changed);
        ui->undoAction->setEnabled(_pixmapTransformState->changed());
        ui->save->setEnabled(_pixmapTransformState->changed());
    }
}

void LayerInstrumentalForm::LayerInstrumentalForm::areaSetted()
{
    _pixmapTransformState->setMode(StateMode::ScrollMap);
    ui->setArea->setChecked(false);
    ui->setClearArea->setEnabled(true);
    ui->fillRB->setEnabled(true);

    bool changed(true);
    ui->apply->setEnabled(changed);
    ui->undoAction->setEnabled(_pixmapTransformState->changed());
    ui->save->setEnabled(_pixmapTransformState->changed());
}

void LayerInstrumentalForm::clearArea()
{
    _pixmapTransformState->clearAreaOnImage();
    ui->setClearArea->setEnabled(false);

    changeColorRBdependElementsEnabled(true);
    ui->fillRB->setChecked(false);
    ui->fillRB->setEnabled(false);

    ui->setTransparent->setText(QString::fromUtf8("на прозрачный"));
    ui->setColor->setText(QString::fromUtf8("на цвет"));
}

void LayerInstrumentalForm::applyForImageOrArea(bool on_off)
{
    if(on_off)
    {
        if(sender() == ui->changeColorRB)
        {
            changeColorRBdependElementsEnabled(true);
            ui->setTransparent->setText(QString::fromUtf8("на прозрачный"));
            ui->setColor->setText(QString::fromUtf8("на цвет"));
        }
        else if(sender() == ui->fillRB)
        {
            changeColorRBdependElementsEnabled(false);
            ui->setTransparent->setText(QString::fromUtf8("прозрачным"));
            ui->setColor->setText(QString::fromUtf8("цветом"));
        }
    }
}

void LayerInstrumentalForm::pixmapChanged()
{
    ui->undoAction->setEnabled(_pixmapTransformState->changed());
    ui->save->setEnabled(_pixmapTransformState->changed());
}

void LayerInstrumentalForm::undoAction()
{
    _pixmapTransformState->undoAction();
}

void LayerInstrumentalForm::save()
{
    UndoAct currentParams = _pixmapTransformState->getCurrentParams();
    emit signalSaved(currentParams.filePath, currentParams.scenePos, currentParams.scaleW, currentParams.scaleH, currentParams.rotation);
}

void LayerInstrumentalForm::changeColorRBdependElementsEnabled(bool on_off)
{
    if(ui->changeColorRB->isChecked() != on_off)
        ui->changeColorRB->setChecked(on_off);
    ui->sensWidget->setEnabled(on_off);
    ui->setColorOutFromImage->setEnabled(on_off);
    ui->setColorOutFromDialog->setEnabled(on_off);
}

void LayerInstrumentalForm::setColorMode(bool on_off)
{
    if(on_off)
    {
        if(sender() == ui->setTransparent)
        {
            ui->setColorInFromDialog->setEnabled(false);
            ui->setColorInFromImage->setEnabled(false);
        }
        else if(sender() == ui->setColor)
        {
            ui->setColorInFromDialog->setEnabled(true);
            ui->setColorInFromImage->setEnabled(true);
        }
    }
}

void LayerInstrumentalForm::pressSetColor()
{
    if(sender() == ui->setColorInFromDialog)
    {
        QColor color = QColorDialog::getColor(_colorIn, this);
        if(color.isValid())
        {
            _colorIn = color;
            QPixmap colorPixmap(32,32);
            colorPixmap.fill(_colorIn);
            ui->setColorInFromDialog->setIcon(QIcon(colorPixmap));
        }
    }
    else if(sender() == ui->setColorOutFromDialog)
    {
        QColor color = QColorDialog::getColor(_colorOut, this);
        if(color.isValid())
        {
            _colorOut = color;
            QPixmap colorPixmap(32,32);
            colorPixmap.fill(_colorOut);
            ui->setColorOutFromDialog->setIcon(QIcon(colorPixmap));
        }
    }
}






















