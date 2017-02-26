#ifndef EMBEDDEDMAINWINDOWHEADER_H
#define EMBEDDEDMAINWINDOWHEADER_H

#include "embeddedheader.h"

namespace ew {

class EmbeddedMainWindowHeader : public EmbeddedHeader
{
    Q_OBJECT

public:
    explicit EmbeddedMainWindowHeader(QWidget * parent = 0);
};
}

#endif // EMBEDDEDMAINWINDOWHEADER_H
