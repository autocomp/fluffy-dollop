#include "slide_window.h"

SlideWindow::SlideWindow()
{
    // params of animation and style
    setMouseTracking( true );
    _animation = new QPropertyAnimation( this, "size" );
}

SlideWindow *SlideWindow::mainInstance()
{
    static SlideWindow* sw = nullptr;
    if( !sw )
        sw = new SlideWindow();

    return sw;
}

void SlideWindow::initGeometry()
{
    setSmallSize( QSize( 10, 80 ));
    setBigSize( QSize( 60, 160 ));
    updateGeometry();
    setType( SlideWindow::ST_RIGHT );
    setDuration( 150 );
    setViewProportionsl( 0.1 );
    show();
}

void SlideWindow::setType( SlideWindow::SlideType type )
{
    _type = type;

    // style
    setObjectName( "SlideWindow" );
    QString corner_radius = cornerRadiusByType();
    setStyleSheet( "QWidget#SlideWindow {" +
                   corner_radius +
                   "border-color: rgb(128, 128, 128);"
                   "border-width: 1px;"
                   "border-style: solid;"
                   "}" );

    // add layout
    if( layout() )
        delete layout();

    if( _type == ST_LEFT
        || _type == ST_RIGHT )
        setLayout( new QVBoxLayout( this ));
    else
        setLayout( new QHBoxLayout( this ));
}

void SlideWindow::setHolder(QWidget *holder)
{
    _holder = holder;
    setParent( _holder );
}

void SlideWindow::setBigSize( QSize size )
{
    _big_size = size;
}

void SlideWindow::setSmallSize( QSize size )
{
    _small_size = size;
}

void SlideWindow::setRadius( uint radius )
{
    _radius = radius;
}

void SlideWindow::setViewProportionsl( double prop )
{
    _prop = prop;
}

void SlideWindow::setDuration(uint dur)
{
    _duration = dur;
}

void SlideWindow::updateGeometry()
{
    if( _showed_slide )
        setGeometry( 0, 0, _big_size.width(), _big_size.height());
    else
        setGeometry( 0, 0, _small_size.width(), _small_size.height());
}

void SlideWindow::addWidget( QWidget* wdgt )
{
    layout()->addWidget( wdgt );
}

bool SlideWindow::event( QEvent* e )
{
    if( e->type() == QEvent::Enter )
        showSlide();
    else if( e->type() == QEvent::Leave )
        hideSlide();
    else if( e->type() == QEvent::Paint )
        onResizeWidget();

    return QWidget::event( e );
}

void SlideWindow::showSlide()
{
    if( !_showed_slide )
    {
        _animation->stop();
        _animation->setDuration( _duration );
        _animation->setStartValue( _small_size );
        _animation->setEndValue( _big_size );
        _animation->start();

        _showed_slide = true;
    }
}

void SlideWindow::hideSlide()
{
    if( _showed_slide )
    {
        _animation->stop();
        _animation->setDuration( _duration );
        _animation->setStartValue( _big_size );
        _animation->setEndValue( _small_size );
        _animation->start();

        _showed_slide = false;
    }
}

void SlideWindow::onResizeWidget()
{
    if( _holder )
    {
        QSize parent_size = _holder->size();
        QPoint tl = QPoint( parent_size.width() - geometry().width(),
                            parent_size.height() - geometry().height());
        // moved in true way
        if( _type == ST_LEFT )
            setGeometry( 0, tl.y() * _prop, size().width(), size().height());
        else if( _type == ST_RIGHT )
            setGeometry( tl.x(), tl.y() * _prop, size().width(), size().height());
        else if( _type == ST_TOP )
            setGeometry( tl.x() * _prop, 0, size().width(), size().height());
        else if( _type == ST_BOTTOM )
            setGeometry( tl.x() * _prop, tl.y(), size().width(), size().height());
    }
}

QString SlideWindow::cornerRadiusByType()
{
    QString corners;
    if( _type == ST_LEFT )
        corners =  "border-bottom-right-radius:" + QString::number( _radius ) + "px;" +
                  "border-top-right-radius:" + QString::number( _radius ) + "px;";
    else if( _type == ST_RIGHT )
        corners =  "border-bottom-left-radius:" + QString::number( _radius ) + "px;" +
                  "border-top-left-radius:" + QString::number( _radius ) + "px;";
    else if( _type == ST_TOP )
        corners =  "border-bottom-left-radius:" + QString::number( _radius ) + "px;" +
                  "border-bottom-right-radius:" + QString::number( _radius ) + "px;";
    else if( _type == ST_BOTTOM )
        corners =  "border-top-left-radius:" + QString::number( _radius ) + "px;" +
                  "border-top-right-radius:" + QString::number( _radius ) + "px;";

    return corners;
}
