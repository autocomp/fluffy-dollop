#ifndef CTRCORE_GLOBAL_H
#define CTRCORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CTRCORE_LIBRARY)
#  define CTRCORESHARED_EXPORT Q_DECL_EXPORT
#else
#  define CTRCORESHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // CTRCORE_GLOBAL_H
