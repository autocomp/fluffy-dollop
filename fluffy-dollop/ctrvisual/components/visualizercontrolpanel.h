#ifndef VISUALIZERCONTROLPANEL_H
#define VISUALIZERCONTROLPANEL_H

#include <QWidget>
#include <QToolButton>
#include <QMenu>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStateMachine>
#include <QState>
#include <QPropertyAnimation>
#include <ctrcore/plugin/embifacenotifier.h>

namespace visualize_system {


class VisualizerControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit VisualizerControlPanel(QWidget *parent);
    ~VisualizerControlPanel();

    QAction *addItem(const QString &itemName = "", const QString &itemTitle = "", const QIcon &itemIcon = QIcon(), const QString &itemToolTip = "", bool chekable = false);
    QAction *addMenuItem(QList<QAction *> menuItems, const QString &menuTitle, const QIcon &menuIcon, const QString &menuToolTip);
    void paintEvent(QPaintEvent *pe);
    const QSize &getItemSize();
    int getPanelWidth();
    int count();
    QIcon getMenuIcon();
    void init(int visId = 0);

public slots:
    void setTitleVisible(bool tf);
    void setItemSize(uint w, uint h);
    void addSeparator();
    void setPluginButtonEnabled(const QString & buttonName, bool on_off);
    void initAnimationStates();
    void hidePanel();
    void showPanel();
    void movePanel(int x, int y);

protected:
    EmbIFaceNotifier * _iface = nullptr;
    QHash<QAction*, QToolButton*> hAllButtons;
    QMap<QString,QMenu*> mAllMenus;
    QStateMachine machine_;
    QState *st1 = nullptr;
    QState *st2 = nullptr;
    QPropertyAnimation* an1 = nullptr;
    QHBoxLayout *mainLayout = nullptr;
    QToolButton *ctrBtnHide = nullptr;
    QToolButton *ctrBtnShow = nullptr;
    QSize itemSize;
    int panelWidth;
    bool isTitleVisible = false;

signals:

    void needShow();
    void needHide();

    void visibleChanged(bool);
};
}
#endif // VISUALIZERCONTROLPANEL_H
