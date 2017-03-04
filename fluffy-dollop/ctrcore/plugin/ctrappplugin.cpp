#include "ctrappplugin.h"
#include <QDebug>

CtrAppPlugin::CtrAppPlugin()
{
}

CtrAppPlugin::~CtrAppPlugin()
{
}

void CtrAppPlugin::showPluginInterface(bool)
{
    qDebug() << "showPluginInterface";
}
