#ifndef FILTER_FORM_H
#define FILTER_FORM_H

#include <QWidget>

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

    void registrateToSlideWindow();

private Q_SLOTS:
    void showFilterWindow();
    void resetFilters();

private:
    Ui::FilterForm *ui;
};

#endif // FILTER_FORM_H
