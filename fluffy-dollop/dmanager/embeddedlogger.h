#ifndef EMBEDDEDLOGGER_H
#define EMBEDDEDLOGGER_H
class QDebug;
class QString;
#include <QString>
#include <QDebug>
#include <QWidgetList>
#include <QApplication>
#include <QAbstractButton>
#include <QDir>

#include <QListWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QTreeWidget>
#include <QPlainTextEdit>



//! \defgroup LOG Логгирование
//! \ingroup LOG
#define ewInitDebug(x)   ew::EmbeddedLogger::instance()->setLoggerFuncDebug  (x) //!< Установка функции логгирования отладочной информации
#define ewInitWarning(x) ew::EmbeddedLogger::instance()->setLoggerFuncWarning(x) //!< Установка функции логгирования предупреждений
#define ewInitFatal(x)   ew::EmbeddedLogger::instance()->setLoggerFuncFatal  (x) //!< Установка функции логгирования критических ошибок
#define ewInitActionLogger(x) ew::EmbeddedActionLogger::instance()->setLoggerFunc(x) //!< Установка функции фиксации действий пользователя
#define ewDebug()   *(ew::EmbeddedLogger::instance()->debug())  //!< Логгированиe отладочной информации
#define ewWarning() *(ew::EmbeddedLogger::instance()->warning())//!< Логгированиe предупреждений
#define ewFatal()   *(ew::EmbeddedLogger::instance()->fatal())  //!< Логгированиe критических ошибок


namespace ew {

//! \ingroup LOG
class Log
{
    Log(Log const&);
    void operator =(Log const&);
    void (*_log_func)(QString); //!< Переменная хранения функции логгирования
public:
    Log():_log_func(nullptr){}
    Log& operator<<(const QString t){log(t); return *(this);}   //! \todo return this
    void setLogFunc(void (*log_func)(QString));
    void log(QString str);

    void log( int value){log(QString::number(value));}
    void log( uint value){log(QString::number(value));}
    void log( long value){log(QString::number(value));}
    void log( ulong value){log(QString::number(value));}
    void log( qlonglong value){log(QString::number(value));}
    void log( qulonglong value){log(QString::number(value));}
    void log( double value){log(QString::number(value));}

};

//! \brief Класс осуществляющий связь с внешним логером
//! \ingroup LOG
//! \version 1.0.0
//! \author Савельев С.Б.
//! \details Перед использованием логгера, необходимо
//! проинициализировать разные типы логгеров соответствующими функциями.
//! \code
//! ew::EmbeddedLogger::instance()->setLoggerFuncDebug(eDebug);
//! \endcode
class EmbeddedLogger
{
    friend class EmbeddedApp;

    Log *_debug  ; //!< Переменная хранения функции логгирования отладочной информации
    Log *_warning; //!< Переменная хранения функции логгирования предупреждений
    Log *_fatal  ; //!< Переменная хранения функции логгирования критических ошибок

    EmbeddedLogger();             //!< Приватный конструктор

    // Защита от копирования
    EmbeddedLogger (EmbeddedLogger const&); //!< Не реализовывать
    void operator=(EmbeddedLogger const&);  //!< Не реализовывать


protected:
    void setLoggerFuncDebug  (void (*logger_func)(QString)); //!< Установка функции логгирования отладочной информации
    void setLoggerFuncWarning(void (*logger_func)(QString)); //!< Установка функции логгирования предупреждений
    void setLoggerFuncFatal  (void (*logger_func)(QString)); //!< Установка функции логгирования критических ошибок
public:
    static EmbeddedLogger * instance(); //!< Получения указателя на класс
    inline Log *debug  (){return _debug;}  //!< Получение указателя на логгер отладочной информации
    inline Log *warning(){return _warning;}//!< Получение указателя на логгер предупреждений
    inline Log *fatal  (){return _fatal;}  //!< Получение указателя на логгер критических ошибок
};


//! \brief Занимается сохранением действий пользователя
//! \details Когда окно становится активным, логгер сохраняет состояние его заполненных контроллов.
//! при выполнении пользователем действий над контроллами логгер сохраняет изменения.
//! \ingroup LOG
class EmbeddedActionLogger : public QObject
{
    Q_OBJECT
    void (*_logger_func)(QString); //!< Переменная хранящая указатель на функцию логгирования
    EmbeddedActionLogger ();
    //Защита от копирования
    EmbeddedActionLogger operator =(EmbeddedActionLogger const&);// Не реализовывать
    EmbeddedActionLogger (EmbeddedActionLogger const&);// Не реализовывать

    QWidget * m_currentRootWidget;
    QWidgetList m_widgetList;

public:
    //! Получение указателя на логгер (Синглтон)
    static EmbeddedActionLogger * instance();

    /// \brief Установка функции логгирования
    /// \param logger_func Указатель на функцию обработчик
    void setLoggerFunc(void (*logger_func)(QString));

    /// \brief Логгирование действий
    /// \param log Строка с записанными действиями.
    void log(QString log);

    ///
    /// \brief getWidgetState Функция получает состояние виджета и формирует строку для сохранения.
    /// \param widget виджет состояние которого нужно сохранить
    /// \return строка состояния.
    ///
    QString getWidgetState(QWidget * widget, int level=0);

    /// \brief Функция генерирует имя виджета для логгирования на основе имени класса и объектного имени
    /// \param wdg Указатель на виджет
    /// \return Имя
    QString getWidgetName(QWidget *wdg);

    /// \brief Генерирует имя виджета включая всю иерархию
    /// \param wdg Указатель на виджет
    /// \return Имя
    QString getWidgetFullName(QWidget *wdg);

    /// \brief Возвращает разделитель для имени
    /// \return Разделитель
    QString getNameSeparator();

    /// \brief Выполняет коннект к слотам логгирования
    /// \param Указатель на виджет
    /// \todo rename
    void connect_(QWidget *wdg);

public slots:
    /// \brief Срабатывает при изменении фокуса виджета
    /// \param old Старый
    /// \param now Новый
    void	slotFocusChanged ( QWidget * old, QWidget * now );



    void slotQTreeWidget_currentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
    void slotQTreeWidget_itemChanged ( QTreeWidgetItem * item, int column );
    void slotQListWidget_itemChanged ( QListWidgetItem * item );
    void slotQListWidget_currentRowChanged ( int currentRow);
    void slotQTextEdit_textChanged();
    void slotQPlainTextEdit_textChanged();
    void slotQTableWidget_itemChanged ( QTableWidgetItem * item );
    void slotQTableWidget_currentCellChanged(int a,int b,int c,int d);


    // Слоты сгенерированные по списку используя регулярное выражение
    //    void slotQAbstractButton_clicked(bool state){log((QString("<action click=\"%1=%2\"").arg(getWidgetFullName((QWidget*)sender()),state?"1":"0")).append("/>"));}
    //^[ \t]*//[ \t]*([a-z]*)[ \t]*([a-z]*)( *\([a-z ,&\*]*\))$
    //void slot\1_\2()\n{\nlog((QString("<action sender=\\"%1\\" signal=\\"%2\\"").arg(getWidgetFullName((QWidget*)sender()),"\2")).append("/>"));\n}

    void slotQAbstractButton_clicked();
    void slotQAbstractSlider_actionTriggered();
    void slotQAbstractSlider_rangeChanged();
    void slotQAbstractSlider_sliderMoved();
    void slotQAbstractSlider_sliderPressed();
    void slotQAbstractSlider_sliderReleased();
    void slotQAbstractSlider_valueChanged();
    void slotQAbstractSpinBox_editingFinished();
    void slotQCalendarWidget_selectionChanged();
    void slotQComboBox_activated();
    void slotQComboBox_currentIndexChanged();
    void slotQComboBox_editTextChanged();
    void slotQComboBox_highlighted();
    void slotQDialog_accepted();
    void slotQDialog_finished();
    void slotQDialog_rejected();
    void slotQDialogButtonBox_accepted();
    void slotQDialogButtonBox_clicked();
    void slotQDialogButtonBox_helpRequested();
    void slotQDialogButtonBox_rejected();
    void slotQLineEdit_cursorPositionChanged();
    void slotQLineEdit_editingFinished();
    void slotQLineEdit_returnPressed();
    void slotQLineEdit_selectionChanged();
    void slotQLineEdit_textChanged();
    void slotQLineEdit_textEdited();
    void slotQMenu_triggered();
    void slotQMenuBar_triggered();
    void slotQProgressBar_valueChanged();
    void slotQStatusBar_messageChanged();
    void slotQTabBar_currentChanged();
    void slotQTabBar_tabCloseRequested();
    void slotQTabBar_tabMoved();
    void slotQTabWidget_currentChanged();
    void slotQTabWidget_tabCloseRequested();
    void slotQToolBar_actionTriggered();
    void slotQWebView_linkClicked();
    void slotQWebView_urlChanged();


// список использованных сигналов для генерации слотов

    // QAbstractSlider         	actionTriggered ( int action )
    // QAbstractSlider         	rangeChanged ( int min, int max )
    // QAbstractSlider         	sliderMoved ( int value )
    // QAbstractSlider         	sliderPressed ()
    // QAbstractSlider         	sliderReleased ()
    // QAbstractSlider         	valueChanged ( int value )
    // QAbstractSpinBox   editingFinished()
    // QCalendarWidget   selectionChanged ()
    //QComboBox  	activated ( int index )
    //QComboBox  	activated ( const QString & text )
    //QComboBox  	currentIndexChanged ( int index )
    //QComboBox  	currentIndexChanged ( const QString & text )
    //QComboBox  	editTextChanged ( const QString & text )
    //QComboBox  	highlighted ( int index )
    //QComboBox  	highlighted ( const QString & text )
    // QDialog	accepted ()
    // QDialog	finished ( int result )
    // QDialog	rejected ()
    // QDialogButtonBox	accepted ()
    // QDialogButtonBox	clicked ( QAbstractButton * button )
    // QDialogButtonBox	helpRequested ()
    // QDialogButtonBox	rejected ()
    // QLineEdit	cursorPositionChanged ( int old, int new )
    // QLineEdit	editingFinished ()
    // QLineEdit	returnPressed ()
    // QLineEdit	selectionChanged ()
    // QLineEdit	textChanged ( const QString & text )
    // QLineEdit	textEdited ( const QString & text )
    // QMenu 	triggered ( QAction * action )
    // QMenuBar triggered ( QAction * action )
    //  QProgressBar valueChanged ( int value )
    // QStatusBar messageChanged ( const QString & message )
    // QTabBar 	currentChanged ( int index )
    // QTabBar 	tabCloseRequested ( int index )
    // QTabBar 	tabMoved ( int from, int to )
    //  QTabWidget 	currentChanged ( int index )
    // QTabWidget 	tabCloseRequested ( int index )
    // QToolBar 	actionTriggered ( QAction * action )
    // QWebView	linkClicked ( const QUrl & url )
    // QWebView	urlChanged ( const QUrl & url )
    //// QToolBar 	allowedAreasChanged ( Qt::ToolBarAreas allowedAreas )
    //// QToolBar 	iconSizeChanged ( const QSize & iconSize )
    //// QToolBar 	movableChanged ( bool movable )
    //// QToolBar 	orientationChanged ( Qt::Orientation orientation )
    //// QToolBar 	toolButtonStyleChanged ( Qt::ToolButtonStyle toolButtonStyle )
    //// QToolBar 	topLevelChanged ( bool topLevel )
    //// QToolBar 	visibilityChanged ( bool visible )
    //// QWebView	iconChanged ()
    //// QWebView	loadFinished ( bool ok )
    //// QWebView	loadProgress ( int progress )
    //// QWebView	loadStarted ()
    //// QWebView	selectionChanged ()
    //// QWebView	statusBarMessage ( const QString & text )
    //// QWebView	titleChanged ( const QString & title )
    //// QWizardPage
    ////    QFrame


};
}




#endif // EMBEDDEDLOGGER_H
