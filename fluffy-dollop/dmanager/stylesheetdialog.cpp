#include "stylesheetdialog.h"
#include <QDir>
#include <QDialogButtonBox>
#include <QPushButton>
#include "embeddedapp.h"

using namespace ew;


StylesheetDialog::StylesheetDialog(const QString path, QWidget *parent) :
    QDialog(parent),
    EmbeddedSubIFace(),
    dir(path)
{
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("StylesheetDialog"));
//    resize(264, 385);
//    QWidget* main = new QWidget(this);
//    this->setWidget(main);
    verticalLayout = new QVBoxLayout(this);
//    main->setLayout(verticalLayout);
    verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    listWidget = new QListWidget(this);
    listWidget->setObjectName(QString::fromUtf8("listWidget"));

    verticalLayout->addWidget(listWidget);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    verticalLayout->addWidget(buttonBox);

    QObject::connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(listWidget, SIGNAL(itemSelectionChanged()),
            this, SLOT(onItemSelectionChanged()));

    listWidget->clear();
    listWidget->addItems(dir.entryList(QStringList()<<"*.qss"));
    m_result = "";


    raise();

    //checkCorrect();

}

StylesheetDialog::~StylesheetDialog()
{
    verticalLayout->deleteLater();
    listWidget    ->deleteLater();
    buttonBox     ->deleteLater();
}

void StylesheetDialog::setstylesheetPath(QString path)
{
    dir.setPath(path);
}

QString StylesheetDialog::selectedStyleFile()
{
    return m_result;
}

void StylesheetDialog::onItemSelectionChanged()
{
    m_result = "";
    QList<QListWidgetItem*> li = listWidget->selectedItems();
    foreach (QListWidgetItem* l , li)
    {
        m_result = dir.path()+QDir::separator()+l->text();
        //checkCorrect();
    }
    //    qDebug()<<m_result;
}
