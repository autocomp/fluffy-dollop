#ifndef FILTER_FORM_H
#define FILTER_FORM_H

#include <QWidget>

#include <ctrcore/plugin/embifacenotifier.h>

namespace Ui {
class FilterForm;
}

class FilterForm : public QWidget
{
    Q_OBJECT

public:
    explicit FilterForm(QWidget *parent = 0);
    ~FilterForm();
    static FilterForm& instance();

    void setParentId( quint64 parent_id ){ _parentWidgetId = parent_id; }
    void registrateToSlideWindow();

private Q_SLOTS:
    void showFilterWindow();
    void resetFilters();

    void on_pushButton_reset_clicked();
    void on_pushButton_apply_clicked();
    void on_pushButton_ok_clicked();

private:
    void hideWindow();

    Ui::FilterForm *ui;

    quint64 _parentWidgetId = 0;
    EmbIFaceNotifier* _ifaceInfoWidget = nullptr;
};

#endif // FILTER_FORM_H
