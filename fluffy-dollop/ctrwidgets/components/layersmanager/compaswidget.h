#ifndef COMPASWIDGET_H
#define COMPASWIDGET_H

#include <QLabel>

class CompasWidget : public QLabel
{
public:
    CompasWidget();
    void setAngle(int angle);
};

#endif // COMPASWIDGET_H
