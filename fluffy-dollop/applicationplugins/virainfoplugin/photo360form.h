#ifndef PHOTO360FORM_H
#define PHOTO360FORM_H

#include <QWidget>

namespace Ui {
class Photo360Form;
}

class Photo360Form : public QWidget
{
    Q_OBJECT

public:
    explicit Photo360Form();
    ~Photo360Form();
    void showWidget(quint64 id);

signals:
    void signalCloseWindow();

private slots:
    void slotCloseWindow();

private:
    Ui::Photo360Form *ui;

    // это как-бы просмотрщик панорамных фото
    // Photo360Viewer * _photo360Viewer;
};

#endif // PHOTO360FORM_H
