#ifndef STYLESHEETDIALOG_H
#define STYLESHEETDIALOG_H

#include <QVariant>
#include <QAction>
#include <QApplication>
#include <QButtonGroup>
#include <QDialog>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QListWidget>
#include <QVBoxLayout>
#include <QDir>
#include "embeddedsubiface.h"

namespace ew {

class StylesheetDialog: public QDialog, public ew::EmbeddedSubIFace
{
    Q_OBJECT

    QVBoxLayout         *verticalLayout;
    QListWidget         *listWidget;
    QDialogButtonBox    *buttonBox;
    QDir dir;
    QString m_result;

public:

    explicit StylesheetDialog(const QString path = ":/embwidgets/stylesheets/stylesheets", QWidget *parent = 0);
    ~StylesheetDialog();
    void setstylesheetPath(const QString path);
    QString selectedStyleFile();
    QWidget* getWidget(){return this;}
private:

private slots:
    void onItemSelectionChanged();

};
}

#endif // STYLESHEETDIALOG_H
