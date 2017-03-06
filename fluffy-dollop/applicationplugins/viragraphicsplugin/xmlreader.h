#ifndef XMLREADER_H
#define XMLREADER_H

#include "types.h"

namespace pdf_editor
{

class XmlReader
{
public:
    static bool readFacility(const QString& xmlFile, Facility& facility);
    static bool readLocation(const QString& xmlFile, Location& location);
};

}

#endif // XMLREADER_H
