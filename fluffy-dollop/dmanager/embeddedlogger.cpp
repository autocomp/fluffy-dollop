#include "embeddedlogger.h"
#include <QDebug>
#include <QWidgetList>
#include <QWidget>
#include <QPushButton>
#include "embeddedheader.h"

#include <QMenu>
#include <QAbstractSlider>
#include <QAbstractSpinBox>
#include <QCalendarWidget>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTabWidget>
#include <QStatusBar>
#include <QProgressBar>
#include <QMenuBar>
#include <QToolBar>

#ifndef QT_V5
#include <QWebView>
#else
//#include <QWebEnginePage>
//#include <QWebEngineView>
#endif

#include <QPlainTextEdit>
ew::EmbeddedLogger::EmbeddedLogger()
    : _debug  (new ew::Log())
    , _warning(new ew::Log())
    , _fatal  (new ew::Log())
{

}

ew::EmbeddedLogger *ew::EmbeddedLogger::instance()
{
    static ew::EmbeddedLogger *self(new ew::EmbeddedLogger);
    return self;
}


void  ew::EmbeddedLogger::setLoggerFuncDebug(void (*logger_func)(QString))
{
    _debug->setLogFunc(logger_func);
}

void ew::EmbeddedLogger::setLoggerFuncWarning(void (*logger_func)(QString))
{
    _warning->setLogFunc(logger_func);
}

void ew::EmbeddedLogger::setLoggerFuncFatal(void (*logger_func)(QString))
{
    _fatal->setLogFunc(logger_func);
}


void ew::Log::setLogFunc(void (*log_func)(QString))
{
    _log_func=log_func;
}

void ew::Log::log(QString str)
{
    if(_log_func)
    {
        _log_func(str);
    }
    else
    {
        static bool logNotSetWarning (false); // Переменная будет инициализирована один раз;
        if(!logNotSetWarning)
        {
            qWarning()<<QObject::tr("Logger function not set. Use \"setLoggerFunc( func_ptr )\".");
            logNotSetWarning = true;
        }
        qDebug()<<str;
    }
}


ew::EmbeddedActionLogger::EmbeddedActionLogger()
    : QObject(0)
    , _logger_func(0)
    , m_currentRootWidget(nullptr)
{
}

ew::EmbeddedActionLogger *ew::EmbeddedActionLogger::instance()
{
    static EmbeddedActionLogger *self(new EmbeddedActionLogger());
    return self;
}

void ew::EmbeddedActionLogger::setLoggerFunc(void (*logger_func)(QString))
{
    _logger_func = logger_func;
}

void ew::EmbeddedActionLogger::log(QString log)
{
    if(_logger_func)
    {
        _logger_func(log);
    }
//    else
//    {
//        qDebug()<<log;
//    }
}

QString ew::EmbeddedActionLogger::getWidgetState(QWidget *widget, int level)
{

    if(widget)
    {
        QString space; // Отступ
        for (int i = 0; i < level; ++i) {space.append(' ');}
        QString c_name = widget->metaObject()->className(); // Имя класса
        QString o_name = widget->objectName(); // Имя объекта

        QString string =QString("%space<w id=\"%1==%2\"")
                .arg(c_name,o_name);
        auto addAtr = [&](QString name, QString value)
        {
            string.append(QString(" %1=\"%2\"").arg(name,value));
        };

        QObjectList wList = widget->children();


//QWidget props
        {
            addAtr("height", QString::number(widget->height()));
            addAtr("toolTip", widget->toolTip());
            addAtr("width", QString::number(widget->width()));
            addAtr("windowIcon", widget->windowIcon().name());
            addAtr("windowIconText", widget->windowIconText());
            addAtr("windowTitle", widget->windowTitle());
            QString s = widget->windowTitle();
            addAtr("x", QString::number(widget->x()));
            addAtr("y", QString::number(widget->y()));
        }

        bool casted = false;
        {
            ew::EmbeddedHeader* header = qobject_cast<ew::EmbeddedHeader*>(widget);
            if(header)
            {
                casted = true;
                addAtr("title",header->text());
            }
        }
        if(!casted)
        {
            QAbstractButton * wPtr = qobject_cast<QPushButton*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("text",wPtr->text());
                addAtr("icon",wPtr->icon().name());
                addAtr("isChecked",wPtr->isChecked()?"1":"0");
            }
        }
        if(!casted)
        {
            QMenu * wPtr = qobject_cast<QMenu*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("title",wPtr->title());
                addAtr("icon",wPtr->icon().name());
            }
        }
        if(!casted)
        {
            QAbstractSlider* wPtr = qobject_cast<QAbstractSlider*>(widget);
            if(wPtr)
            {
                casted = true;
                //Generate bool from properties
                //'tracking : bool'->'addAtr("tracking", (wPtr->tracking())?"\"1\"":"\"0\"");'

                //bool
                //from:     '^( *)([a-z]{1,}) : bool$'
                //to:       '\1addAtr("\2", (wPtr->\2())?"\"1\"":"\"0\"");'

                //int
                //from:     '^( *)([a-z]{1,}) : (const ){,1}int$'
                //to:       '\1addAtr("\2", QString::number(wPtr->\2()));'

                //QString
                //^( *)([a-z]{1,}) : (const ){,1}QString
                //\1addAtr("\2", wPtr->\2());

                //other
                //^( *)([a-z]{1,}) : Qt::.*$
                //\1addAtr("\2", QString::number((int)wPtr->\2()));

                    addAtr("maximum", QString::number(wPtr->maximum()));
                    addAtr("minimum", QString::number(wPtr->minimum()));
                    addAtr("pageStep", QString::number(wPtr->pageStep()));
                    addAtr("singleStep", QString::number(wPtr->singleStep()));
                    addAtr("sliderPosition", QString::number(wPtr->sliderPosition()));
                    addAtr("value", QString::number(wPtr->value()));
            }
        }
        if(!casted)
        {
            QAbstractSpinBox* wPtr = qobject_cast<QAbstractSpinBox*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("text", wPtr->text());
            }
        }
        if(!casted)
        {
            QCalendarWidget* wPtr = qobject_cast<QCalendarWidget*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("selectedDate",wPtr->selectedDate().toString("yyyyMMdd"));
            }
        }
        if(!casted)
        {
            QComboBox* wPtr = qobject_cast<QComboBox*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("currentIndex", QString::number(wPtr->currentIndex()));
                addAtr("currentText", wPtr->currentText());
            }
        }
        if(!casted)
        {
            QDialogButtonBox* wPtr = qobject_cast<QDialogButtonBox*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("standardButtons", QString::number((int)wPtr->standardButtons()));
            }
        }
        if(!casted)
        {
            QLineEdit* wPtr = qobject_cast<QLineEdit*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("displayText", wPtr->displayText());
                addAtr("inputMask", wPtr->inputMask());
                addAtr("modified", (wPtr->isModified())?"\"1\"":"\"0\"");
                addAtr("selectedText", wPtr->selectedText());
                addAtr("text", wPtr->text());
            }
        }
        if(!casted)
        {
            QTabWidget* wPtr = qobject_cast<QTabWidget*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("currentIndex", QString::number(wPtr->currentIndex()));
            }
        }
        if(!casted)
        {
            QProgressBar* wPtr = qobject_cast<QProgressBar*>(widget);
            if(wPtr)
            {
                casted = true;
                addAtr("maximum", QString::number(wPtr->maximum()));
                addAtr("minimum", QString::number(wPtr->minimum()));
                addAtr("text", wPtr->text());
                addAtr("value", QString::number(wPtr->value()));
            }
        }
        string.append(">\n");
        foreach (QObject* ob, wList)
        {
            QWidget* w =qobject_cast<QWidget*>(ob);
            if(w)
            {
                string.append(getWidgetState(w,level+1));
            }
        }
        string.append("%space</w>\n");
        string.replace("%space",space);
        return string;
    }
    return "";
}


QString ew::EmbeddedActionLogger::getWidgetName(QWidget *wdg)
{
    return QString(wdg->metaObject()->className()) + "_" + wdg->objectName();
}


QString ew::EmbeddedActionLogger::getWidgetFullName(QWidget *wdg)
{
    QString clickedWidget = getWidgetName(wdg);
    QWidget *parentWdg = wdg->parentWidget();

    while(parentWdg)
    {
        clickedWidget.prepend( QString(parentWdg->metaObject()->className()) + "_" + parentWdg->objectName() + getNameSeparator() );
        parentWdg = parentWdg->parentWidget();
    }

    return clickedWidget;
}


QString ew::EmbeddedActionLogger::getNameSeparator()
{
    return QDir::separator();
}


void ew::EmbeddedActionLogger::connect_(QWidget *wdg)
{
    {
        QTableWidget *qab = qobject_cast<QTableWidget *>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(currentCellChanged(int,int,int,int)),SLOT(slotQTableWidget_currentCellChanged(int,int,int,int)));
            connect(qab,SIGNAL(itemChanged(QTableWidgetItem*)),SLOT(slotQTableWidget_itemChanged(QTableWidgetItem*)));
        }
    }

    {
        QTextEdit * w = qobject_cast<QTextEdit *>(wdg);
        if (w)
        {
            connect (w,SIGNAL(textChanged()),SLOT(slotQTextEdit_textChanged()));
        }
    }

    {
        QPlainTextEdit * w = qobject_cast<QPlainTextEdit *>(wdg);
        if (w)
        {
            connect (w,SIGNAL(textChanged()),SLOT(slotQPlainTextEdit_textChanged()));
        }
    }

    {
        QTreeWidget* w = qobject_cast<QTreeWidget *>(wdg);
        if(w)
        {
            connect(w,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),SLOT(slotQTreeWidget_currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
            connect(w,SIGNAL(itemChanged(QTreeWidgetItem*,int)),SLOT(slotQTreeWidget_itemChanged(QTreeWidgetItem*,int)));
        }
    }

    {
        QListWidget * w = qobject_cast<QListWidget *>(wdg);
        if(w)
        {
            connect(w,SIGNAL(currentRowChanged(int)),SLOT(slotQListWidget_currentRowChanged(int)));
            connect(w,SIGNAL(itemChanged(QListWidgetItem*)),SLOT(slotQListWidget_itemChanged(QListWidgetItem*)));
        }
    }

    //generated
    //^[ \t]*//[ \t]*([a-z]*)[ \t]*([a-z]*)( *\([a-z ,&\*]*\))$
    //{\n\1 *qab = qobject_cast<\1*>(wdg);\nif(qab)\n{\nconnect(qab,SIGNAL(\2\3),SLOT(slot\1_\2()));\n}\n}\n

    {
        QAbstractSlider *qab = qobject_cast<QAbstractSlider*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(actionTriggered ( int  )),SLOT(slotQAbstractSlider_actionTriggered()));
        }
    }

    {
        QAbstractSlider *qab = qobject_cast<QAbstractSlider*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(rangeChanged ( int , int  )),SLOT(slotQAbstractSlider_rangeChanged()));
        }
    }

    {
        QAbstractSlider *qab = qobject_cast<QAbstractSlider*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(sliderMoved ( int  )),SLOT(slotQAbstractSlider_sliderMoved()));
        }
    }

    {
        QAbstractSlider *qab = qobject_cast<QAbstractSlider*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(sliderPressed ()),SLOT(slotQAbstractSlider_sliderPressed()));
        }
    }

    {
        QAbstractSlider *qab = qobject_cast<QAbstractSlider*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(sliderReleased ()),SLOT(slotQAbstractSlider_sliderReleased()));
        }
    }

    {
        QAbstractSlider *qab = qobject_cast<QAbstractSlider*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(valueChanged ( int  )),SLOT(slotQAbstractSlider_valueChanged()));
        }
    }

    {
        QAbstractSpinBox *qab = qobject_cast<QAbstractSpinBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(editingFinished()),SLOT(slotQAbstractSpinBox_editingFinished()));
        }
    }

    {
        QCalendarWidget *qab = qobject_cast<QCalendarWidget*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(selectionChanged ()),SLOT(slotQCalendarWidget_selectionChanged()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(activated ( int  )),SLOT(slotQComboBox_activated()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(activated ( const QString &  )),SLOT(slotQComboBox_activated()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(currentIndexChanged ( int  )),SLOT(slotQComboBox_currentIndexChanged()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(currentIndexChanged ( const QString &  )),SLOT(slotQComboBox_currentIndexChanged()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(editTextChanged ( const QString &  )),SLOT(slotQComboBox_editTextChanged()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(highlighted ( int  )),SLOT(slotQComboBox_highlighted()));
        }
    }

    {
        QComboBox *qab = qobject_cast<QComboBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(highlighted ( const QString &  )),SLOT(slotQComboBox_highlighted()));
        }
    }

    {
        QDialog *qab = qobject_cast<QDialog*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(accepted ()),SLOT(slotQDialog_accepted()));
        }
    }

    {
        QDialog *qab = qobject_cast<QDialog*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(finished ( int  )),SLOT(slotQDialog_finished()));
        }
    }

    {
        QDialog *qab = qobject_cast<QDialog*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(rejected ()),SLOT(slotQDialog_rejected()));
        }
    }

    {
        QDialogButtonBox *qab = qobject_cast<QDialogButtonBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(accepted ()),SLOT(slotQDialogButtonBox_accepted()));
        }
    }

    {
        QDialogButtonBox *qab = qobject_cast<QDialogButtonBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(clicked ( QAbstractButton *  )),SLOT(slotQDialogButtonBox_clicked()));
        }
    }

    {
        QDialogButtonBox *qab = qobject_cast<QDialogButtonBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(helpRequested ()),SLOT(slotQDialogButtonBox_helpRequested()));
        }
    }

    {
        QDialogButtonBox *qab = qobject_cast<QDialogButtonBox*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(rejected ()),SLOT(slotQDialogButtonBox_rejected()));
        }
    }

    {
        QLineEdit *qab = qobject_cast<QLineEdit*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(cursorPositionChanged ( int , int  )),SLOT(slotQLineEdit_cursorPositionChanged()));
        }
    }

    {
        QLineEdit *qab = qobject_cast<QLineEdit*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(editingFinished ()),SLOT(slotQLineEdit_editingFinished()));
        }
    }

    {
        QLineEdit *qab = qobject_cast<QLineEdit*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(returnPressed ()),SLOT(slotQLineEdit_returnPressed()));
        }
    }

    {
        QLineEdit *qab = qobject_cast<QLineEdit*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(selectionChanged ()),SLOT(slotQLineEdit_selectionChanged()));
        }
    }

    {
        QLineEdit *qab = qobject_cast<QLineEdit*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(textChanged ( const QString &  )),SLOT(slotQLineEdit_textChanged()));
        }
    }

    {
        QLineEdit *qab = qobject_cast<QLineEdit*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(textEdited ( const QString &  )),SLOT(slotQLineEdit_textEdited()));
        }
    }

    {
        QMenu *qab = qobject_cast<QMenu*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(triggered ( QAction *  )),SLOT(slotQMenu_triggered()));
        }
    }

    {
        QMenuBar *qab = qobject_cast<QMenuBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(triggered ( QAction *  )),SLOT(slotQMenuBar_triggered()));
        }
    }

    {
        QProgressBar *qab = qobject_cast<QProgressBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(valueChanged ( int  )),SLOT(slotQProgressBar_valueChanged()));
        }
    }

    {
        QStatusBar *qab = qobject_cast<QStatusBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(messageChanged ( const QString &  )),SLOT(slotQStatusBar_messageChanged()));
        }
    }

    {
        QTabBar *qab = qobject_cast<QTabBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(currentChanged ( int  )),SLOT(slotQTabBar_currentChanged()));
        }
    }

    {
        QTabBar *qab = qobject_cast<QTabBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(tabCloseRequested ( int  )),SLOT(slotQTabBar_tabCloseRequested()));
        }
    }

    {
        QTabBar *qab = qobject_cast<QTabBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(tabMoved ( int , int  )),SLOT(slotQTabBar_tabMoved()));
        }
    }

    {
        QTabWidget *qab = qobject_cast<QTabWidget*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(currentChanged ( int  )),SLOT(slotQTabWidget_currentChanged()));
        }
    }

    {
        QTabWidget *qab = qobject_cast<QTabWidget*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(tabCloseRequested ( int  )),SLOT(slotQTabWidget_tabCloseRequested()));
        }
    }

    {
        QToolBar *qab = qobject_cast<QToolBar*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(actionTriggered ( QAction *  )),SLOT(slotQToolBar_actionTriggered()));
        }
    }
/*
#ifndef QT_V5
    {
        QWebView *qab = qobject_cast<QWebView*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(linkClicked ( const QUrl &  )),SLOT(slotQWebView_linkClicked()));
        }
    }

    {
        QWebView *qab = qobject_cast<QWebView*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(urlChanged ( const QUrl &  )),SLOT(slotQWebView_urlChanged()));
        }
    }

#else
    {
        QWebEngineView *qab = qobject_cast<QWebEngineView*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(urlChanged(QUrl)),SLOT(slotQWebView_linkClicked()));
        }
    }
#endif
*/
    {
        QAbstractButton * qab = qobject_cast<QAbstractButton*>(wdg);
        if(qab)
        {
            connect(qab,SIGNAL(clicked(bool)),SLOT(slotQAbstractButton_clicked()));
        }
    }


    foreach(QObject* o, wdg->children())
    {
        QWidget * w = qobject_cast<QWidget*>(o);
        if(w)
        {
            connect_(w);
        }
    }
}

void ew::EmbeddedActionLogger::slotFocusChanged(QWidget * /*old*/, QWidget * /*now*/)
{
    QWidgetList tops = qApp->topLevelWidgets();
    foreach(QWidget * w ,tops)
    {
        if(w->isActiveWindow())
        {
            if(m_currentRootWidget != w)
            {

                QString action(
                            "<action type=\"window_selected\">\n"
                            "%1"
                            "</action>\n");
                action=action.arg(getWidgetState(w,1));
                m_currentRootWidget = w;

                log(action);
                qApp->processEvents();

                disconnect(this,SLOT(slotQTreeWidget_currentItemChanged ( QTreeWidgetItem * , QTreeWidgetItem *  )));
                disconnect(this,SLOT(slotQTreeWidget_itemChanged ( QTreeWidgetItem * , int  )));
                disconnect(this,SLOT(slotQListWidget_itemChanged ( QListWidgetItem *  )));
                disconnect(this,SLOT(slotQListWidget_currentRowChanged ( int )));
                disconnect(this,SLOT(slotQTextEdit_textChanged()));
                disconnect(this,SLOT(slotQPlainTextEdit_textChanged()));
                disconnect(this,SLOT(slotQTableWidget_itemChanged ( QTableWidgetItem *  )));
                disconnect(this,SLOT(slotQTableWidget_currentCellChanged(int ,int ,int ,int )));


                disconnect(this,SLOT(slotQAbstractButton_clicked()));
                //generated disconnect
                //^[ \t]*//[ \t]*([a-z]*)[ \t]*([a-z]*)( *\([a-z ,&\*]*\))$
                //disconnect(this,SLOT(slot\1_\2()));

                disconnect(this,SLOT(slotQAbstractSlider_actionTriggered()));
                disconnect(this,SLOT(slotQAbstractSlider_rangeChanged()));
                disconnect(this,SLOT(slotQAbstractSlider_sliderMoved()));
                disconnect(this,SLOT(slotQAbstractSlider_sliderPressed()));
                disconnect(this,SLOT(slotQAbstractSlider_sliderReleased()));
                disconnect(this,SLOT(slotQAbstractSlider_valueChanged()));
                disconnect(this,SLOT(slotQAbstractSpinBox_editingFinished()));
                disconnect(this,SLOT(slotQCalendarWidget_selectionChanged()));
                disconnect(this,SLOT(slotQComboBox_activated()));
                disconnect(this,SLOT(slotQComboBox_currentIndexChanged()));
                disconnect(this,SLOT(slotQComboBox_editTextChanged()));
                disconnect(this,SLOT(slotQComboBox_highlighted()));
                disconnect(this,SLOT(slotQComboBox_highlighted()));
                disconnect(this,SLOT(slotQDialog_accepted()));
                disconnect(this,SLOT(slotQDialog_finished()));
                disconnect(this,SLOT(slotQDialog_rejected()));
                disconnect(this,SLOT(slotQDialogButtonBox_accepted()));
                disconnect(this,SLOT(slotQDialogButtonBox_clicked()));
                disconnect(this,SLOT(slotQDialogButtonBox_helpRequested()));
                disconnect(this,SLOT(slotQDialogButtonBox_rejected()));
                disconnect(this,SLOT(slotQLineEdit_cursorPositionChanged()));
                disconnect(this,SLOT(slotQLineEdit_editingFinished()));
                disconnect(this,SLOT(slotQLineEdit_returnPressed()));
                disconnect(this,SLOT(slotQLineEdit_selectionChanged()));
                disconnect(this,SLOT(slotQLineEdit_textChanged()));
                disconnect(this,SLOT(slotQLineEdit_textEdited()));
                disconnect(this,SLOT(slotQMenu_triggered()));
                disconnect(this,SLOT(slotQMenuBar_triggered()));
                disconnect(this,SLOT(slotQProgressBar_valueChanged()));
                disconnect(this,SLOT(slotQStatusBar_messageChanged()));
                disconnect(this,SLOT(slotQTabBar_currentChanged()));
                disconnect(this,SLOT(slotQTabBar_tabCloseRequested()));
                disconnect(this,SLOT(slotQTabBar_tabMoved()));
                disconnect(this,SLOT(slotQTabWidget_currentChanged()));
                disconnect(this,SLOT(slotQTabWidget_tabCloseRequested()));
                disconnect(this,SLOT(slotQToolBar_actionTriggered()));
                disconnect(this,SLOT(slotQWebView_linkClicked()));
                disconnect(this,SLOT(slotQWebView_urlChanged()));

                connect_(w);
            }
            break;
        }
    }
}

void ew::EmbeddedActionLogger::slotQTreeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"currentItemChanged"));
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQTreeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"itemChanged"));
    s.append(QString(" column=\"%1\" text=\"%2\"").arg(column).arg(item->text(column)));
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQListWidget_itemChanged(QListWidgetItem *item)
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"itemChanged"));
    s.append(" text=\"").append(item->text()).append("\"");
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQListWidget_currentRowChanged(int currentRow)
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"currentItemChanged"));
    s.append(" row=\"").append(QString::number(currentRow)).append("\"");
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQTextEdit_textChanged()
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"textChanged"));
    QTextEdit* e =qobject_cast<QTextEdit *>(sender());
    if(e)
    {
        s.append(" text=\"").append(e->toPlainText()).append("\"");
    }
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQPlainTextEdit_textChanged()
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"textChanged"));
    QPlainTextEdit* e =qobject_cast<QPlainTextEdit *>(sender());
    if(e)
    {
        s.append(" text=\"").append(e->toPlainText()).append("\"");
    }
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQTableWidget_itemChanged(QTableWidgetItem *item)
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"itemChanged"));
    if(item)
    {
        s.append(" text=\"").append(item->text()).append("\"");
    }
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQTableWidget_currentCellChanged(int a, int b, int c, int d)
{
    QString s = (QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"currentCellChanged"));
    QTableWidget * w = qobject_cast<QTableWidget *>(sender());
    s.append(QString (" cell=\"%1, %2, %3, %4\"").arg(a).arg(b).arg(c).arg(d));
    s.append("/>");
    log(s);
}

void ew::EmbeddedActionLogger::slotQAbstractButton_clicked()
{
    log((QString("<action sender=\"%1\" text=\"%2\" signal=\"%3\"").arg(getWidgetFullName((QWidget*)sender()),"clicked")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSlider_actionTriggered()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"actionTriggered")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSlider_rangeChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"rangeChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSlider_sliderMoved()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"sliderMoved")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSlider_sliderPressed()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"sliderPressed")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSlider_sliderReleased()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"sliderReleased")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSlider_valueChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"valueChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQAbstractSpinBox_editingFinished()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"editingFinished")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQCalendarWidget_selectionChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"selectionChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQComboBox_activated()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"activated")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQComboBox_currentIndexChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"currentIndexChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQComboBox_editTextChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"editTextChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQComboBox_highlighted()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"highlighted")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialog_accepted()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"accepted")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialog_finished()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"finished")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialog_rejected()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"rejected")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialogButtonBox_accepted()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"accepted")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialogButtonBox_clicked()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"clicked")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialogButtonBox_helpRequested()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"helpRequested")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQDialogButtonBox_rejected()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"rejected")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQLineEdit_cursorPositionChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"cursorPositionChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQLineEdit_editingFinished()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"editingFinished")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQLineEdit_returnPressed()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"returnPressed")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQLineEdit_selectionChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"selectionChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQLineEdit_textChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"textChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQLineEdit_textEdited()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"textEdited")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQMenu_triggered()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"triggered")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQMenuBar_triggered()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"triggered")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQProgressBar_valueChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"valueChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQStatusBar_messageChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"messageChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQTabBar_currentChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"currentChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQTabBar_tabCloseRequested()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"tabCloseRequested")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQTabBar_tabMoved()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"tabMoved")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQTabWidget_currentChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"currentChanged")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQTabWidget_tabCloseRequested()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"tabCloseRequested")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQToolBar_actionTriggered()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"actionTriggered")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQWebView_linkClicked()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"linkClicked")).append("/>"));
}

void ew::EmbeddedActionLogger::slotQWebView_urlChanged()
{
    log((QString("<action sender=\"%1\" signal=\"%2\"").arg(getWidgetFullName((QWidget*)sender()),"urlChanged")).append("/>"));
}


