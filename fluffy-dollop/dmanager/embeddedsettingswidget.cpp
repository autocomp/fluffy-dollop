#include "embeddedsettingswidget.h"

#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QTableWidget>
#include <QCheckBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include "embeddedapp.h"

using namespace ew;

EmbeddedSettingsWidget::EmbeddedSettingsWidget(QWidget *parent) :
    ew::EmbeddedWidget(parent)
{
    m_inited = false;
    setupTableWidget();
}



EmbeddedSettingsWidget::EmbeddedSettingsWidget(EmbeddedWidgetBaseStruct ews, QWidget *parent)
    : ew::EmbeddedWidget(parent)
{
    m_inited = false;
    setupTableWidget();
    fillForm(ews);
}

void EmbeddedSettingsWidget::slotButtonClicked(QAbstractButton* btn)
{
    if(m_pButtonBox->button(QDialogButtonBox::Ok) == btn)
    {
        accept();
    }

    if(m_pButtonBox->button(QDialogButtonBox::Cancel) == btn)
    {
        reject();
    }

    if(m_pButtonBox->button(QDialogButtonBox::Apply) == btn)
    {
        emit signalSettingsApplied();
    }

}


EmbeddedSettingsWidget::~EmbeddedSettingsWidget()
{
}

void EmbeddedSettingsWidget::setupTableWidget()
{

    ew::EmbeddedHeaderStruct hstr;
    hstr.windowTitle =  tr("Settings");
    hstr.hasCloseButton = true;
    hstr.hasCollapseButton = false;
    hstr.hasGlobalMoveButton = false;
    hstr.hasPinButton = false;

    hstr.hasWhatButton = false;
    hstr.hasLockButton = false;
    hstr.hasMinMaxButton = false;

    setHeader(hstr);
    if (objectName().isEmpty())
        setObjectName(QString::fromUtf8("EmbeddedSettingsWidget"));
    resize(264, 385);

    QWidget* main = new QWidget(this);
    this->setWidget(main);
    QVBoxLayout * mainLt = new QVBoxLayout(main);
    main->setLayout(mainLt);
    mainLt->setObjectName(QString::fromUtf8("verticalLayout"));
    if(m_inited)
    {
        return;
    }

    m_inited = true;

    m_pButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel,Qt::Horizontal, this);
    connect(m_pButtonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(slotButtonClicked(QAbstractButton*)));

    m_pTableWidget = new QTableWidget(this);

    mainLt->addWidget(m_pTableWidget);
    mainLt->addWidget(m_pButtonBox);

    m_pTableWidget->setColumnCount(2);

#ifdef QT_V5
    m_pTableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Stretch);
#else
    m_pTableWidget->horizontalHeader()->setResizeMode(1,QHeaderView::Stretch);
#endif

    m_pTableWidget->setRowCount(TAB_ROW_COUNT);
    m_pTableWidget->verticalHeader()->hide();

    QStringList lst;
    lst.append(tr("Name"));
    lst.append(tr("Value"));
    m_pTableWidget->setHorizontalHeaderLabels(lst);


    //    //Хедер
    qchb_hasCloseButton         = new QCheckBox();
    qchb_hasVisibilityButton    = new QCheckBox();
    qchb_hasGlobalMoveButton    = new QCheckBox();
    qchb_hasVisibilityCheckbox  = new QCheckBox();
    qchb_hasCollapseButton      = new QCheckBox();
    qchb_hasPinButton           = new QCheckBox();
    qchb_hasWhatButton          = new QCheckBox();
    qchb_hasLockButton          = new QCheckBox();
    qchb_hasMinMaxButton        = new QCheckBox();
    qle_descr                   = new QLineEdit();
    qle_windowTitle             = new QLineEdit();
    qle_header_tooltip          = new QLineEdit();
    qle_headerPixmap            = new QLineEdit();
    qcob_headerAlign            = new QComboBox();
    qle_header_descr            = new QLineEdit();


    qcob_headerAlign->addItem(QString::fromLocal8Bit("Слева" ),ew::EHA_LEFT  );
    qcob_headerAlign->addItem(QString::fromLocal8Bit("Справа"),ew::EHA_RIGHT );
    qcob_headerAlign->addItem(QString::fromLocal8Bit("Сверху"),ew::EHA_TOP   );
    qcob_headerAlign->addItem(QString::fromLocal8Bit("Снизу" ),ew::EHA_BOTTOM);

    // widget
    qcob_alignType      = new QComboBox      ();

    qcob_alignType->addItem(QString::fromLocal8Bit("Внутренняя привязка левый верхний угол" ),ew::EWA_IN_LEFT_TOP_CORNER_OFFSET     );
    qcob_alignType->addItem(QString::fromLocal8Bit("Внутренняя привязка левый нижний угол"  ),ew::EWA_IN_LEFT_BOTTOM_CORNER_OFFSET  );
    qcob_alignType->addItem(QString::fromLocal8Bit("Внутренняя привязка правый верхний угол"),ew::EWA_IN_RIGHT_TOP_CORNER_OFFSET    );
    qcob_alignType->addItem(QString::fromLocal8Bit("Внутренняя привязка правый нижний угол" ),ew::EWA_IN_RIGHT_BOTTOM_CORNER_OFFSET );
    qcob_alignType->addItem(QString::fromLocal8Bit("Внутренняя привязка по центру"          ),ew::EWA_IN_PERCENTAGE_ALIGN           );
    qcob_alignType->addItem(QString::fromLocal8Bit("Без привязки"                           ),ew::EWA_OUT_FREE_WIDGET               );
    qcob_alignType->addItem(QString::fromLocal8Bit("привязка виджета левый верхний угол"    ),ew::EWA_OUT_LEFT_TOP_CORNER_OFFSET    );
    qcob_alignType->addItem(QString::fromLocal8Bit("привязка виджета левый нижний угол"     ),ew::EWA_OUT_LEFT_BOTTOM_CORNER_OFFSET );
    qcob_alignType->addItem(QString::fromLocal8Bit("привязка виджета правый верхний угол"   ),ew::EWA_OUT_RIGHT_TOP_CORNER_OFFSET   );
    qcob_alignType->addItem(QString::fromLocal8Bit("привязка виджета правый нижний угол"    ),ew::EWA_OUT_RIGHT_BOTTOM_CORNER_OFFSET);

    qdsb_alignPointX    = new QDoubleSpinBox ();
    qdsb_alignPointY    = new QDoubleSpinBox ();
    qchb_addHided       = new QCheckBox      ();
    qchb_allowMoving    = new QCheckBox      ();
    qchb_collapsed      = new QCheckBox      ();
    qchb_locked         = new QCheckBox      ();
    qchb_showWindowTitle= new QCheckBox      ();
    qle_checkedIcon     = new QLineEdit      ();
    qle_descr           = new QLineEdit      ();
    qle_header_tooltip  = new QLineEdit      ();
    qle_uncheckedIcon   = new QLineEdit      ();
    qle_visibleCbText   = new QLineEdit      ();
    qle_widgetTag       = new QLineEdit      ();
    qchb_sizeHeight     = new QSpinBox       ();
    qchb_sizeWidth      = new QSpinBox       ();
    qsb_object_id       = new QSpinBox       ();
    qle_tooltip         = new QLineEdit      ();


    insertProperty(HAS_CLOSE_BUTTON_INDEX        ,QString::fromLocal8Bit("Кнопка \"закрыть\""               ),qchb_hasCloseButton       );
    insertProperty(HAS_VISIBILITY_BUTTON_INDEX   ,QString::fromLocal8Bit("Кнопка \"видимость\""             ),qchb_hasVisibilityButton  );
    insertProperty(HAS_GLOBALMOVE_BUTTON_INDEX   ,QString::fromLocal8Bit("Кнопка \"глобальное перемещение\""),qchb_hasGlobalMoveButton  );
    insertProperty(HAS_VISIBILITY_CHECKBOX_INDEX ,QString::fromLocal8Bit("Флажек \"видимость\""             ),qchb_hasVisibilityCheckbox);
    insertProperty(HAS_COLLAPSE_BUTTON_INDEX     ,QString::fromLocal8Bit("Кнопка \"Свернуть\""              ),qchb_hasCollapseButton    );
    insertProperty(HAS_PIN_BUTTON_INDEX          ,QString::fromLocal8Bit("hasPinButton"                     ),qchb_hasPinButton         );
    insertProperty(HAS_WHAT_BUTTON_INDEX         ,QString::fromLocal8Bit("Кнопка \"Что это?\""              ),qchb_hasWhatButton        );
    insertProperty(HAS_LOCK_BUTTON_INDEX         ,QString::fromLocal8Bit("Кнопка \"зафиксировать\""         ),qchb_hasLockButton        );
    insertProperty(HAS_MINMAX_BUTTON_INDEX       ,QString::fromLocal8Bit("Кнопка \"минимизировать\""        ),qchb_hasMinMaxButton      );
    //    insertProperty(HAS_DESCR_BUTTON_INDEX       ,QString::fromLocal8Bit("Кнопка \"\""        ),qchb_hasDescrButton      );
    insertProperty(DESCR_INDEX                   ,QString::fromLocal8Bit("Краткое описание"                 ),qle_header_descr          );
    insertProperty(WINDOW_TITLE_INDEX            ,QString::fromLocal8Bit("Заглавие окна"                    ),qle_windowTitle           );
    insertProperty(HEADER_TOOLTIP_INDEX          ,QString::fromLocal8Bit("всплывающая подсказка"            ),qle_header_tooltip        );
    //    insertProperty(HEADER_PIXMAP_INDEX           ,QString::fromLocal8Bit("Иконка заголовка"                 ),qle_headerPixmap          );
    insertProperty(HEADER_ALIGN_TYPE_INDEX       ,QString::fromLocal8Bit("Выравнивание заголовка"           ),qcob_headerAlign          );

    insertProperty(ALIGN_TYPE_INDEX       ,QString::fromLocal8Bit("Способ выравнивания"            ),qcob_alignType      );
    insertProperty(ALIGN_POINT_X_INDEX    ,QString::fromLocal8Bit("Точка выравниванияa X"          ),qdsb_alignPointX    );
    insertProperty(ALIGN_POINT_Y_INDEX    ,QString::fromLocal8Bit("Точка выравниванияa У"          ),qdsb_alignPointY    );
    insertProperty(ALLOW_MOVING_INDEX     ,QString::fromLocal8Bit("азрешить захватывать окно"      ),qchb_allowMoving    );
    insertProperty(VISIBLE_CBTEXT_INDEX   ,QString::fromLocal8Bit("подпись к checkbox"             ),qle_visibleCbText   );
    //    insertProperty(CHECKED_ICON_INDEX     ,"checked_Icon"        ,qle_checkedIcon       );
    //    insertProperty(UNCHECKED_ICON_INDEX   ,"unchecked_Icon"      ,qle_uncheckedIcon     );
    insertProperty(TOOLTIP_INDEX          ,QString::fromLocal8Bit("Краткое описание к кнопкам"     ),qle_tooltip         );
    insertProperty(SHOW_WINDOW_TITLE_INDEX,QString::fromLocal8Bit("Отображать заголовок виджета"   ),qchb_showWindowTitle);
    insertProperty(COLLAPSED_INDEX        ,QString::fromLocal8Bit("виджет свернут"                 ),qchb_collapsed      );
    insertProperty(ADD_HIDED_INDEX        ,QString::fromLocal8Bit("добавлять скрытым"              ),qchb_addHided       );
    insertProperty(SIZE_HEIGHT_INDEX      ,QString::fromLocal8Bit("размеры виджета ширина"         ),qchb_sizeWidth      );
    insertProperty(SIZE_WIDTH_INDEX       ,QString::fromLocal8Bit("размеры виджета высота"         ),qchb_sizeHeight     );
    insertProperty(DESCR_INDEX            ,QString::fromLocal8Bit("справка об окне"                ),qle_descr           );
    insertProperty(WIDGET_TAG_INDEX       ,QString::fromLocal8Bit("tag встраиваемого плагина"      ),qle_widgetTag       );
    //    insertProperty(LOCKED_INDEX           ,"locked"              ,qchb_locked           );
    //    insertProperty(OBJECT_ID_INDEX        ,"object_id"           ,qsb_object_id         );

    m_pTableWidget->resizeColumnsToContents();
    //    setWindowTitle(QString::fromLocal8Bit("Настройка окна \"%1\"").arg(qle_windowTitle->text()));
    connect(qle_windowTitle,SIGNAL(textChanged(QString)),SLOT(slotTextChanged(QString)));
    slotTextChanged(qle_windowTitle->text());
}

void EmbeddedSettingsWidget::insertProperty(int row, QString label, QWidget *w)
{
    m_pTableWidget->setItem(row,0,new QTableWidgetItem(label));
    m_pTableWidget->setCellWidget(row,1,w);
    m_pTableWidget->item(row,0)->setFlags(0);

}

void EmbeddedSettingsWidget::slotTextChanged(const QString &text)
{
    if(text.isEmpty())
    {
        setWindowTitle(QString::fromLocal8Bit("Настройка окна, без названия"));
        return;
    }
    setWindowTitle(QString::fromLocal8Bit("Настройка окна \"%1\"").arg(qle_windowTitle->text()));
}

void EmbeddedSettingsWidget::fillForm(ew::EmbeddedWidgetBaseStruct st)
{
    ew::EmbeddedHeaderStruct ehs=st.header;
    qchb_hasCloseButton        ->setChecked(ehs.hasCloseButton)         ;
    //qchb_hasVisibilityButton   ->setChecked(ehs.hasVisibilityButton)    ;
    qchb_hasGlobalMoveButton   ->setChecked(ehs.hasGlobalMoveButton)    ;
    //qchb_hasVisibilityCheckbox ->setChecked(ehs.hasVisibilityCheckbox)  ;
    qchb_hasCollapseButton     ->setChecked(ehs.hasCollapseButton)      ;
    qchb_hasPinButton          ->setChecked(ehs.hasPinButton)           ;
    qchb_hasWhatButton         ->setChecked(ehs.hasWhatButton)          ;
    qchb_hasLockButton         ->setChecked(ehs.hasLockButton)          ;
    qchb_hasMinMaxButton       ->setChecked(ehs.hasMinMaxButton)        ;
    qle_header_descr           ->setText(ehs.descr);
    qle_windowTitle            ->setText(ehs.windowTitle);
    qle_header_tooltip                ->setText(ehs.tooltip);
    //    qle_headerPixmap           ->setText(ehs.headerPixmap.); //WARNING: Не знаю что сюда писать

    //    qcob_headerAlign           ->setCurrentIndex(ehs.align);
    for (int i = 0 ; i < qcob_headerAlign->count() ; i++)
    {
        if(qcob_headerAlign->itemData(i).toInt() == ehs.align)
        {
            qcob_headerAlign->setCurrentIndex(i);
            break;
        }
    }

    //    qcob_alignType->           setCurrentIndex(st.alignType);
    for (int i = 0 ; i < qcob_alignType->count() ; i++)
    {
        if(qcob_alignType->itemData(i).toInt() == st.alignType)
        {
            qcob_alignType->setCurrentIndex(i);
            break;
        }
    }


    qdsb_alignPointX->         setValue(st.alignPoint.x());
    qdsb_alignPointY->          setValue(st.alignPoint.y());
    qchb_allowMoving->          setChecked(st.allowMoving);
    //qle_visibleCbText->         setText(st.visibleCbText);
    //qle_header_tooltip->               setText(st.tooltip);
    //qchb_showWindowTitle->      setChecked(st.showWindowTitle);
    qchb_collapsed->            setChecked(st.collapsed);
    qchb_addHided->             setChecked(st.addHided);
    qchb_sizeWidth->            setValue(st.size.width());
    qchb_sizeHeight->           setValue(st.size.height());
    //qle_descr->                 setText(st.descr);
    qle_widgetTag->             setText(st.widgetTag);
    qchb_locked->               setChecked(st.locked);

}

ew::EmbeddedWidgetStruct EmbeddedSettingsWidget::getResult()
{
    ew::EmbeddedWidgetStruct  ret;
    ret.header.hasCloseButton = qchb_hasCloseButton->isChecked();
    //ret.header.hasVisibilityButton  = qchb_hasVisibilityButton->isChecked();
    ret.header.hasGlobalMoveButton  = qchb_hasGlobalMoveButton->isChecked();
    //ret.header.hasVisibilityCheckbox= qchb_hasVisibilityCheckbox->isChecked();
    ret.header.hasCollapseButton    = qchb_hasCollapseButton->isChecked();
    ret.header.hasPinButton         = qchb_hasPinButton->isChecked();
    ret.header.hasWhatButton        = qchb_hasWhatButton->isChecked();
    ret.header.hasLockButton        = qchb_hasLockButton->isChecked();
    ret.header.hasMinMaxButton      = qchb_hasMinMaxButton->isChecked();
    ret.header.descr        = qle_header_descr->text();
    ret.header.windowTitle  = qle_windowTitle->text();
    ret.header.tooltip      = qle_header_tooltip->text();
    //    ret.header.headerPixmap = qle_headerPixmap;
    ret.header.align        = (ew::EmbeddedHeaderAlign)qcob_headerAlign->currentIndex();

    ret.alignType       = (ew::EmbeddedWidgetAlign)qcob_alignType->currentIndex();
    ret.alignPoint     = QPointF(qdsb_alignPointX->value()
                                 ,qdsb_alignPointY->value());
    ret.allowMoving     = qchb_allowMoving->isChecked();
    //ret.visibleCbText   = qle_visibleCbText->text();
    //    ret.checkedIcon     = qle_checkedIcon
    //    ret.uncheckedIcon   = qle_uncheckedIcon
    //ret.tooltip         = qle_tooltip->text();
    //ret.showWindowTitle = qchb_showWindowTitle->isChecked();
    ret.collapsed       = qchb_collapsed->isChecked();
    ret.addHided        = qchb_addHided->isChecked();
    ret.size            = QSize(qchb_sizeWidth->value()
                                ,qchb_sizeHeight->value());
    //ret.descr           = qle_descr->text();
    ret.widgetTag       = qle_widgetTag->text();
    ret.locked          = qchb_locked->isChecked();
    //    ret.object_id       = qsb_object_id

    return ret;
}


