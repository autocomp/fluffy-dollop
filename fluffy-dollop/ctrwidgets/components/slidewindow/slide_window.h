#ifndef SLIDE_WINDOW_H
#define SLIDE_WINDOW_H

#include <QWidget>
#include <QLayout>
#include <QPropertyAnimation>
#include <QEvent>

class SlideWindow: public QWidget
{
public:
    enum SlideType
    {
        ST_LEFT,
        ST_TOP,
        ST_RIGHT,
        ST_BOTTOM
    };

    SlideWindow();
    static SlideWindow* mainInstance();

    // init
    void initGeometry();

    // make type
    void setType( SlideType type );

    // params of view
    void setHolder( QWidget* holder );
    void updateGeometry();
    void setBigSize( QSize size );
    void setSmallSize( QSize size );
    void setRadius( uint radius );
    void setViewProportionsl( double prop );
    void setDuration( uint dur );

    // widgets
    void addWidget( QWidget* wdgt );

protected:
    bool event(QEvent* e);

private:
    void showSlide();
    void hideSlide();
    void onResizeWidget();
    QString cornerRadiusByType();

    SlideType _type = ST_LEFT;
    uint _radius = 10;
    double _prop = 0.5;
    uint _duration = 100;
    QPropertyAnimation* _animation = nullptr;
    bool _showed_slide = false;
    QSize _big_size;
    QSize _small_size;
    QWidget* _holder = nullptr;
};

#endif // SLIDE_WINDOW_H
