#ifndef EMBEDEDSETTINGSWIDGET_H
#define EMBEDEDSETTINGSWIDGET_H

#include <QDialog>
#include <QMap>

#include "embeddedstruct.h"
#include "embeddedwidget.h"


class QDialogButtonBox;
class QTableWidget;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QTableWidget;
class QDoubleSpinBox;
class QSpinBox;
class QAbstractButton;

namespace ew
{

class EmbeddedSettingsWidget : public ew::EmbeddedWidget
{
    Q_OBJECT

    QVBoxLayout * verticalLayout;
    QTableWidget *m_pTableWidget;
    QDialogButtonBox * m_pButtonBox;
    bool m_inited;

    explicit EmbeddedSettingsWidget(QWidget *parent = 0);
public:

    explicit EmbeddedSettingsWidget(ew::EmbeddedWidgetBaseStruct ews, QWidget *parent = 0);
    ~EmbeddedSettingsWidget();

private:
    void setupTableWidget();// Заполняем дерево


    /**
     * @brief The TableIndexes enum
     * Этот енум используется в основном для определения порядка
     * расположения элементов в таблице и ее размера.
     * Если нужно добавить элемент, то его обязательно нужно
     * добавить в енум.
     */
    enum TableIndexes{
        HAS_CLOSE_BUTTON_INDEX        ,
        HAS_VISIBILITY_BUTTON_INDEX   ,
        HAS_GLOBALMOVE_BUTTON_INDEX   ,
        HAS_VISIBILITY_CHECKBOX_INDEX ,
        HAS_COLLAPSE_BUTTON_INDEX     ,
        HAS_PIN_BUTTON_INDEX          ,
//        HAS_DESCR_BUTTON_INDEX        ,
        HAS_WHAT_BUTTON_INDEX         ,
        HAS_LOCK_BUTTON_INDEX         ,
        HAS_MINMAX_BUTTON_INDEX       ,
        WINDOW_TITLE_INDEX            ,
        HEADER_TOOLTIP_INDEX          ,
//        HEADER_PIXMAP_INDEX           ,
        HEADER_ALIGN_TYPE_INDEX       ,
        ALIGN_TYPE_INDEX              ,
        ALIGN_POINT_X_INDEX           ,
        ALIGN_POINT_Y_INDEX           ,
        ALLOW_MOVING_INDEX            ,
        VISIBLE_CBTEXT_INDEX          ,
//        CHECKED_ICON_INDEX            ,
//        UNCHECKED_ICON_INDEX          ,
        TOOLTIP_INDEX                 ,
        SHOW_WINDOW_TITLE_INDEX       ,
        COLLAPSED_INDEX               ,
        ADD_HIDED_INDEX               ,
        SIZE_HEIGHT_INDEX             ,
        SIZE_WIDTH_INDEX              ,
        DESCR_INDEX                   ,
        WIDGET_TAG_INDEX              ,
//        LOCKED_INDEX                  ,
//        OBJECT_ID_INDEX               ,

        TAB_ROW_COUNT                 ,//количество строк в таблице
    };

    QMap <int,QString> headerAlignMap;
    QMap <int,QString> AlignMap;


    QCheckBox * qchb_hasCloseButton;
    QCheckBox * qchb_hasVisibilityButton;
    QCheckBox * qchb_hasGlobalMoveButton;
    QCheckBox * qchb_hasVisibilityCheckbox;
    QCheckBox * qchb_hasCollapseButton;
    QCheckBox * qchb_hasPinButton;
    QCheckBox * qchb_hasWhatButton;
    QCheckBox * qchb_hasLockButton;
    QCheckBox * qchb_hasMinMaxButton;
    QLineEdit * qle_header_descr;
    QLineEdit * qle_windowTitle;
    QLineEdit * qle_header_tooltip;
    QLineEdit * qle_headerPixmap;
    QComboBox * qcob_headerAlign;

    QComboBox       * qcob_alignType;
    QDoubleSpinBox  * qdsb_alignPointX;
    QDoubleSpinBox  * qdsb_alignPointY;
    QCheckBox       * qchb_allowMoving;
    QLineEdit       * qle_visibleCbText;
    QLineEdit       * qle_checkedIcon;
    QLineEdit       * qle_uncheckedIcon;
    QLineEdit       * qle_tooltip;
    QCheckBox       * qchb_showWindowTitle;
    QCheckBox       * qchb_collapsed;
    QCheckBox       * qchb_addHided;
    QSpinBox        * qchb_sizeWidth;
    QSpinBox        * qchb_sizeHeight;
    QLineEdit       * qle_descr;
    QLineEdit       * qle_widgetTag;
    QCheckBox       * qchb_locked;
    QSpinBox        * qsb_object_id;

    void insertProperty(int row, QString label, QWidget * w);

private slots:
    void slotTextChanged( const QString & text );
    void slotButtonClicked(QAbstractButton* btn);

public:
    /**
     * @brief fillForm Заполняет форму данными из переданной в
     * качестве параметра структуре.
     * @param st структура с исходными значениями
     */
    void fillForm(EmbeddedWidgetBaseStruct st);

    /**
     * @brief getResult Возвращает измененное состояние структуры.
     * Рекомендуется вызывать после испускания сигнала Accepted
     * @return Измененная структура.
     */
    ew::EmbeddedWidgetStruct getResult();


signals:
    void signalSettingsApplied();
};

}

#endif // EMBEDEDSETTINGSWIDGET_H
