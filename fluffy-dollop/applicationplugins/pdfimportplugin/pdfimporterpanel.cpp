#include "pdfimporterpanel.h"
//#include "sceneview.h"
#include <QDebug>
#include <ctrcore/ctrcore/ctrconfig.h>

using namespace visualize_system;

PdfImporterPanel::PdfImporterPanel(QWidget *parent)
    : VisualizerControlPanel(parent)
    , _menu(0)
    , _modeMenu(0)
{
}

void PdfImporterPanel::init(uint visualizerId, quint64 visualizerWindowId, QWidget * sceneWidgetPanel)
{
    //VisualizerControlPanel::init(visualizerId, visualizerWindowId, sceneWidgetPanel);
}

void PdfImporterPanel::createModeButtonGroup()
{
//    _menu = new QMenu(this);
//    connect(_menu, SIGNAL(triggered(QAction*)), this, SLOT(slotModeChanged(QAction*)));
//    QAction * act = _menu->addAction(QIcon(":/img/048_icons_32_tools_mouse.png"), QString::fromUtf8("Режим выделения слоев"));
//    act->setProperty("Mode", (int)SceneView::SELECT_AND_SCROLL);
//    act = _menu->addAction(QIcon(":/img/053_icons_32_tools_metr.png"), QString::fromUtf8("Режим нанесения линейки"));
//    act->setProperty("Mode", (int)SceneView::CREATE_MEASURE_LINE);
//    act = _menu->addAction(QIcon(":/img/049_icons_32_tools_line.png"), QString::fromUtf8("Режим нанесения линии"));
//    act->setProperty("Mode", (int)SceneView::CREATE_LINE);
//    act = _menu->addAction(QIcon(":/img/050_icons_32_tools_arrow.png"), QString::fromUtf8("Режим нанесения стрелки"));
//    act->setProperty("Mode", (int)SceneView::CREATE_ARROW);
//    act = _menu->addAction(QIcon(":/img/051_icons_32_tools_txt.png"), QString::fromUtf8("Режим нанесения текста"));
//    act->setProperty("Mode", (int)SceneView::CREATE_TEXT);
//    act = _menu->addAction(QIcon(":/img/052_icons_32_tools_poligon.png"), QString::fromUtf8("Режим нанесения полигона"));
//    act->setProperty("Mode", (int)SceneView::CREATE_POLYGON);

//    ToolButtonItem *pActionItem = new ToolButtonItem(QIcon(":/img/048_icons_32_tools_mouse.png"), QString::fromUtf8("Режим выделения слоев"), QString::fromUtf8("Режим выделения слоев"), true, ptb);
//    _modeMenu =
//    _modeMenu->setIcon(QIcon(":/img/048_icons_32_tools_mouse.png"));
//    _modeMenu->setMenu(_menu);

//    ptb->addAction(pActionItem);

//    QList<QAction*> lActions;

//    QAction * act = new QAction(0);
//    act->setIcon(QIcon(":/img/048_icons_32_tools_mouse.png"));
//    act->setText(QString::fromUtf8("Режим выделения слоев"));
//    act->setProperty("Mode", (int)SceneView::SELECT_AND_SCROLL);
//    connect(act, SIGNAL(triggered()), this, SLOT(slotModeChanged()));
//    lActions.append(act);

//    act = new QAction(0);
//    act->setIcon(QIcon(":/img/053_icons_32_tools_metr.png"));
//    act->setText(QString::fromUtf8("Режим нанесения линейки"));
//    act->setProperty("Mode", (int)SceneView::CREATE_MEASURE_LINE);
//    connect(act, SIGNAL(triggered()), this, SLOT(slotModeChanged()));
//    lActions.append(act);

//    act = new QAction(0);
//    act->setIcon(QIcon(":/img/049_icons_32_tools_line.png"));
//    act->setText(QString::fromUtf8("Режим нанесения линии"));
//    act->setProperty("Mode", (int)SceneView::CREATE_LINE);
//    connect(act, SIGNAL(triggered()), this, SLOT(slotModeChanged()));
//    lActions.append(act);

//    act = new QAction(0);
//    act->setIcon(QIcon(":/img/050_icons_32_tools_arrow.png"));
//    act->setText(QString::fromUtf8("Режим нанесения стрелки"));
//    act->setProperty("Mode", (int)SceneView::CREATE_ARROW);
//    connect(act, SIGNAL(triggered()), this, SLOT(slotModeChanged()));
//    lActions.append(act);

//    act = new QAction(0);
//    act->setIcon(QIcon(":/img/051_icons_32_tools_txt.png"));
//    act->setText(QString::fromUtf8("Режим нанесения текста"));
//    act->setProperty("Mode", (int)SceneView::CREATE_TEXT);
//    connect(act, SIGNAL(triggered()), this, SLOT(slotModeChanged()));
//    lActions.append(act);

//    act = new QAction(0);
//    act->setIcon(QIcon(":/img/052_icons_32_tools_poligon.png"));
//    act->setText(QString::fromUtf8("Режим нанесения полигона"));
//    act->setProperty("Mode", (int)SceneView::CREATE_POLYGON);
//    connect(act, SIGNAL(triggered()), this, SLOT(slotModeChanged()));
//    lActions.append(act);

//    _modeMenu = addMenuItem(lActions, QString::fromUtf8("Режим выделения слоев"), QIcon(":/img/048_icons_32_tools_mouse.png"), QString::fromUtf8("Режим выделения слоев"));
}

void PdfImporterPanel::setMode(int mode)
{
    if(_menu)
    {
//        switch(mode)
//        {
//        case SceneView::SELECT_AND_SCROLL : _modeMenu->setIcon(QIcon(":/img/048_icons_32_tools_mouse.png")); break;
//        case SceneView::CREATE_MEASURE_LINE : _modeMenu->setIcon(QIcon(":/img/053_icons_32_tools_metr.png")); break;
//        case SceneView::CREATE_LINE : _modeMenu->setIcon(QIcon(":/img/049_icons_32_tools_line.png")); break;
//        case SceneView::CREATE_ARROW : _modeMenu->setIcon(QIcon(":/img/050_icons_32_tools_arrow.png")); break;
//        case SceneView::CREATE_TEXT : _modeMenu->setIcon(QIcon(":/img/051_icons_32_tools_txt.png")); break;
//        case SceneView::CREATE_POLYGON : _modeMenu->setIcon(QIcon(":/img/052_icons_32_tools_poligon.png")); break;
//        }
    }
}

void PdfImporterPanel::slotModeChanged()
{
    QAction * act = dynamic_cast<QAction *>(sender());
    if(act)
    {
        _modeMenu->setIcon(act->icon());
        QVariant var = act->property("Mode");
        if(var.isValid())
            emit changeMode(var.toInt());
    }
}

void PdfImporterPanel::readSettings()
{
    QString key = "report_doc_panel.menuSettings.buttonSize";
    QString sizeString;
    sizeString = GET_VALUE_BY_NAME(key, "32x32", true).toString();
    itemSize = QSize(sizeString.mid(0,sizeString.indexOf("x")).toInt(), sizeString.mid(sizeString.indexOf("x")+1).toInt());

    setItemSize(itemSize.width(), itemSize.height());

    panelWidth = ((itemSize.width() + 2) * count()) + 4;
    setFixedHeight(itemSize.height() + 2);
}
