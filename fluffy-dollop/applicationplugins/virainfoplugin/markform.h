#ifndef MARKFORM_H
#define MARKFORM_H

#include <QWidget>
#include <QListWidget>
#include <QPixmap>

namespace Ui {
class MarkForm;
}

class MarkFormListWidget;

class MarkForm : public QWidget
{
    Q_OBJECT

public:
    explicit MarkForm(QWidget *parent = 0);
    ~MarkForm();

    void showWidget(quint64 id);

signals:
    void signalCloseWindow();
    void signalUpdateMark(quint64 id);

private slots:
    void slotLoadImage();
    void slotApply();

private:
    Ui::MarkForm *ui;
    quint64 _id;
    MarkFormListWidget * _listWidget;
    QList<QPixmap> _pixmaps;
};

class MarkFormListWidget : public QListWidget
{
public:
    MarkFormListWidget();
    void addItem(const QPixmap& pm);

protected:
    virtual void resizeEvent(QResizeEvent * e);

private:
    QSize _iconSize;
    double _coef = 1;
};

#endif // MARKFORM_H
