#ifndef MINIMAPCONTAINERWIDGET_H
#define MINIMAPCONTAINERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class MiniMapContainerWidget : public QWidget
{
    Q_OBJECT
    QVBoxLayout * m_pMainLayout, *m_pInnerLayout;
    const int m_boundWidth;
public:
    explicit MiniMapContainerWidget(QWidget *parent = 0);
    QWidget * setMiniMap(QWidget * minimapWidget);
    
private:
    QWidget * m_minimapWidget;
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
signals:
    
public slots:
    
};

#endif // MINIMAPCONTAINERWIDGET_H
