#ifndef QSLIDINGWINDOW_GLOBAL_H
#define QSLIDINGWINDOW_GLOBAL_H

//#include <QtCore/qglobal.h>

#if defined(QSLIDINGWINDOW_LIBRARY)
#  define QSLIDINGWINDOWSHARED_EXPORT Q_DECL_EXPORT
#else
#  define QSLIDINGWINDOWSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // QSLIDINGWINDOW_GLOBAL_H
