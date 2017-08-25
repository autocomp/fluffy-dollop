#include "svgeditorform.h"
#include "ui_svgeditorform.h"

#include "svgeditorstate.h"
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <QColorDialog>
#include <QPen>
#include <QTimer>
#include <QDebug>

SvgEditorForm::SvgEditorForm(uint visualizerId, bool onTop)
    : ew::EmbeddedSubIFace()
    , ui(new Ui::SvgEditorForm)
    , _visualizerId(visualizerId)
{
    ui->setupUi(this);

    ui->selectMode->setIcon(QIcon(":/img/icon_select_mode.png"));
    connect(ui->selectMode, SIGNAL(clicked(bool)), this, SLOT(slotChangeMode(bool)));

    ui->addLineMode->setIcon(QIcon(":/img/icon_line_mode.png"));
    connect(ui->addLineMode, SIGNAL(clicked(bool)), this, SLOT(slotChangeMode(bool)));

    ui->addPolygonMode->setIcon(QIcon(":/img/icon_polygon.png"));
    connect(ui->addPolygonMode, SIGNAL(clicked(bool)), this, SLOT(slotChangeMode(bool)));

    ui->addRectMode->setIcon(QIcon(":/img/icon_rect_mode.png"));
    connect(ui->addRectMode, SIGNAL(clicked(bool)), this, SLOT(slotChangeMode(bool)));
    ui->addRectMode->hide();

    ui->addEllipseMode->setIcon(QIcon(":/img/icon_ellipse_mode.png"));
    connect(ui->addEllipseMode, SIGNAL(clicked(bool)), this, SLOT(slotChangeMode(bool)));
    ui->addEllipseMode->hide();

    ui->addAnnotationMode->setIcon(QIcon(":/img/icon_annotation_mode.png"));
    connect(ui->addAnnotationMode, SIGNAL(clicked(bool)), this, SLOT(slotChangeMode(bool)));
    ui->addAnnotationMode->hide();

    QPixmap pm(30,30);
    pm.fill(_strokeDefaultParams.lineColor);
    ui->setLineColor->setIcon(QIcon(pm));
    connect(ui->setLineColor, SIGNAL(clicked()), this, SLOT(slotSetLineColor()));

    ui->setFillColor->setIcon(QIcon(pm));
    connect(ui->setFillColor, SIGNAL(clicked()), this, SLOT(slotSetFillColor()));

    ui->setFont->setIcon(QIcon("://img/icon_textstyle.png"));
    connect(ui->setFont, SIGNAL(clicked()), this, SLOT(slotSetFont()));

    ui->deleteLayer->setIcon(QIcon(":/img/icon_delete.png"));

    ui->saveSvgDoc->setIcon(QIcon(":/img/icon_save.png"));
    connect(ui->saveSvgDoc, SIGNAL(clicked()), this, SLOT(slotSaveSvgDoc()));

    connect(ui->setLineWidth, SIGNAL(valueChanged(int)), this, SLOT(slotLineWidhChanged(int)));
    connect(ui->opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(slotOpasityChanged(int)));
    connect(ui->fillCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotFill(bool)));
    connect(ui->setLineStyle, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLineStyleChanged(int)));

    ui->lineWidget->setVisible(false);
    ui->fillWidget->setVisible(false);
    ui->colorWidget->setVisible(false);
    ui->fontWidget->setVisible(false);

    _svgEditorState = QSharedPointer<svg_editor_state::SvgEditorState>(new svg_editor_state::SvgEditorState(onTop));
    visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(_visualizerId);
    connect(_svgEditorState.data(), SIGNAL(getDefaultStrokeParams(svg_editor_state::StrokeParams&)),
            this, SLOT(slotGetDefaultStrokeParams(svg_editor_state::StrokeParams&)));
    connect(ui->deleteLayer, SIGNAL(clicked()), _svgEditorState.data(), SLOT(deleteSelectedItem()));
    connect(_svgEditorState.data(), SIGNAL(setEnabledDeleteButton(bool)), this, SLOT(slotSetEnabledDeleteButton(bool)));
    connect(_svgEditorState.data(), SIGNAL(setEnabledSaveButton(bool)), this, SLOT(slotSetEnabledSaveButton(bool)));
    connect(_svgEditorState.data(), SIGNAL(svgDocSaved(QString,QPointF)), this, SIGNAL(svgDocSaved(QString,QPointF)));
    if(stateInterface)
        stateInterface->setVisualizerState(_svgEditorState);

    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

SvgEditorForm::~SvgEditorForm()
{
    delete ui;

    if(_svgEditorState)
    {
        _svgEditorState->emit_closeState();
        _svgEditorState.clear();
    }
}

QWidget *SvgEditorForm::getWidget()
{
    return this;
}

void SvgEditorForm::closed(bool *acceptFlag)
{
    if(acceptFlag)
    {
        // *acceptFlag = false;
        emit signalClosed();
    }
}

void SvgEditorForm::slotChangeMode(bool on_off)
{
    bool colorWidgetVisible(true);
    bool lineWidgetVisible(false);
    bool fillWidgetVisible(false);
    bool fontWidgetVisible(false);
    if(on_off)
    {
        if(sender() == ui->selectMode)
        {
            _svgEditorState->setMode(svg_editor_state::StateMode::SelectMode);
            colorWidgetVisible = false;
        }
        else
            ui->selectMode->setChecked(false);

        if(sender() == ui->addLineMode)
        {
            _svgEditorState->setMode(svg_editor_state::StateMode::AddLineMode);
            lineWidgetVisible = true;
        }
        else
            ui->addLineMode->setChecked(false);

        if(sender() == ui->addPolygonMode)
        {
            _svgEditorState->setMode(svg_editor_state::StateMode::AddPolygonMode);
            lineWidgetVisible = true;
            fillWidgetVisible = true;
        }
        else
            ui->addPolygonMode->setChecked(false);

        if(sender() == ui->addRectMode)
        {
            _svgEditorState->setMode(svg_editor_state::StateMode::AddRectMode);
            lineWidgetVisible = true;
            fillWidgetVisible = true;
        }
        else
            ui->addRectMode->setChecked(false);

        if(sender() == ui->addEllipseMode)
        {
            _svgEditorState->setMode(svg_editor_state::StateMode::AddEllipseMode);
            lineWidgetVisible = true;
            fillWidgetVisible = true;
        }
        else
            ui->addEllipseMode->setChecked(false);

        if(sender() == ui->addAnnotationMode)
        {
            _svgEditorState->setMode(svg_editor_state::StateMode::AddAnnotationMode);
            fontWidgetVisible = true;
        }
        else
            ui->addAnnotationMode->setChecked(false);
    }
    else
    {
        ui->selectMode->setChecked(true);
        _svgEditorState->setMode(svg_editor_state::StateMode::SelectMode);
    }

    ui->colorWidget->setVisible(colorWidgetVisible);
    ui->lineWidget->setVisible(lineWidgetVisible);
    ui->fillWidget->setVisible(fillWidgetVisible);
    ui->fontWidget->setVisible(fontWidgetVisible);
    //! тут надо вставить в оба виджета дефолтовые значения !!!!

    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

void SvgEditorForm::slotFill(bool on_off)
{
    _strokeDefaultParams.fillArea = on_off;
    _svgEditorState->setDefaultStrokeParams(_strokeDefaultParams);
}

void SvgEditorForm::slotLineStyleChanged(int index)
{
    qDebug() << "slotLineStyleChanged, index:" << index;

    _strokeDefaultParams.lineStyle = (Qt::PenStyle)(index+1);
    _svgEditorState->setDefaultStrokeParams(_strokeDefaultParams);
}

void SvgEditorForm::slotSetFont()
{

}

void SvgEditorForm::slotMakeAdjustForm()
{
    QWidget * wgt = this;
    QSize wgtMinSize = wgt->minimumSize();
    QSize wgtMaxSize = wgt->maximumSize();
    wgt->setFixedWidth(wgt->size().width());
    adjustSize();
    wgt->resize(0,0);
    ewApp()->adjustWidgetSize(id());
    wgt->setMinimumWidth(wgtMinSize.width());
    wgt->setMaximumWidth(wgtMaxSize.width());
}

void SvgEditorForm::slotDeleteLayer()
{

}

void SvgEditorForm::slotSaveSvgDoc()
{
    _svgEditorState->saveSvgDoc();
}

void SvgEditorForm::slotSetLineColor()
{
    if( false ) // если редактор в режиме "SelectMode" и есть выделенный слой
    {


    }
    else // если редактор НЕ в режиме "SelectMode"
    {
        QColor color = QColorDialog::getColor(_strokeDefaultParams.lineColor, this);
        if(color.isValid())
        {
            _strokeDefaultParams.lineColor = color;
            QPixmap pm(30,30);
            pm.fill(_strokeDefaultParams.lineColor);
            ui->setLineColor->setIcon(QIcon(pm));
            _svgEditorState->setDefaultStrokeParams(_strokeDefaultParams);
        }
    }
}

void SvgEditorForm::slotSetFillColor()
{
    if( false ) // если редактор в режиме "SelectMode" и есть выделенный слой
    {


    }
    else // если редактор НЕ в режиме "SelectMode"
    {
        QColor color = QColorDialog::getColor(_strokeDefaultParams.fillColor, this);
        if(color.isValid())
        {
            _strokeDefaultParams.fillColor = color;
            QPixmap pm(30,30);
            pm.fill(_strokeDefaultParams.fillColor);
            ui->setFillColor->setIcon(QIcon(pm));
            _svgEditorState->setDefaultStrokeParams(_strokeDefaultParams);
        }
    }
}

void SvgEditorForm::slotGetDefaultStrokeParams(svg_editor_state::StrokeParams & strokeParams)
{
    strokeParams = _strokeDefaultParams;
}

void SvgEditorForm::slotLineWidhChanged(int lineWidth)
{
    _strokeDefaultParams.lineWidth = lineWidth;
    _svgEditorState->setDefaultStrokeParams(_strokeDefaultParams);
}

void SvgEditorForm::slotOpasityChanged(int percent)
{
    _strokeDefaultParams.opacity = 0.01 * percent;
    _svgEditorState->setDefaultStrokeParams(_strokeDefaultParams);
}

void SvgEditorForm::slotSetEnabledDeleteButton(bool on_off)
{
    ui->deleteLayer->setEnabled(on_off);
}

void SvgEditorForm::slotSetEnabledSaveButton(bool on_off)
{
    ui->saveSvgDoc->setEnabled(on_off);
}






























