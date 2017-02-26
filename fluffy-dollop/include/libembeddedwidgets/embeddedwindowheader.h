#ifndef EMBEDDEDWINDOWHEADER_H
#define EMBEDDEDWINDOWHEADER_H

#include <QWidget>
#include <QPushButton>
#include <QMenuBar>
#include <QHBoxLayout>
#include <QLabel>

#include "embeddedwidgetheader.h"

namespace ew {

class EmbeddedWindowHeader : public EmbeddedWidgetHeader
{
    Q_OBJECT
    QMenuBar * m_pInstalledMenu;
    int getButtonSpace();
//    QIcon getButtonIcon(WindowHeaderButtons button);

public:
    explicit EmbeddedWindowHeader(QWidget *parent = 0);
    void setMenuBar(QMenuBar * mainMenu);


};
}

#endif // EMBEDDEDWINDOWHEADER_H
