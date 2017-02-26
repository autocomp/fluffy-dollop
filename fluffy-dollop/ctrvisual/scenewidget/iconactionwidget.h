#ifndef ICONACTIONWIDGET_H
#define ICONACTIONWIDGET_H

#include <QWidget>
#include <QIcon>
#include <QWidgetAction>
#include <QPixmap>
#include <QLabel>

/**
 * @brief Класс кнопки выпадающего списка кнопок панели задач для внутреннего использования
 *
 */
class IconActionWidget : public QWidgetAction
{
    Q_OBJECT
    QIcon m_icon;
    QLabel * m_lblIcon ;
    QString m_text;
    QWidget * m_wdg;
    int m_clickedCount;
    bool m_isEnabled;
    bool isVisisble;


public:
    explicit IconActionWidget(QIcon icon, QString &text, QWidget *parent = 0);
    QWidget* createWidget(QWidget *parent) ;
    void setIcon(const QIcon& icon);
    void setEnabled(bool isEnabled);
    void setVisible(bool isVis);

    //QWidget * defaultWidget () const;


signals:
    
public slots:
    
};

#endif // ICONACTIONWIDGET_H
