#ifndef VIRAVISUALIZER2DFORM_H
#define VIRAVISUALIZER2DFORM_H

#include <QWidget>

class QStackedWidget;

namespace Ui {
class ViraVisualizer2dForm;
}

class ViraVisualizer2dForm : public QWidget
{
public:
    explicit ViraVisualizer2dForm(QWidget *parent = 0);
    ~ViraVisualizer2dForm();
    QStackedWidget * getStackedWidget();
    void addWidgetToSplitterLeftArea(QWidget * wdg);
    void addWidgetToStatusBar(QWidget * wdg);

private:
    Ui::ViraVisualizer2dForm *ui;
};

#endif // VIRAVISUALIZER2DFORM_H
