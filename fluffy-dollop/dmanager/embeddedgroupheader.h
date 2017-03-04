#ifndef EMBEDDEDGROUPHEADER_H
#define EMBEDDEDGROUPHEADER_H

#include "embeddedheader.h"

namespace ew {

class EmbeddedGroupHeader : public EmbeddedHeader
{
    Q_OBJECT

public:
    explicit EmbeddedGroupHeader(QWidget * parent);
};
}

#endif // EMBEDDEDGROUPHEADER_H
