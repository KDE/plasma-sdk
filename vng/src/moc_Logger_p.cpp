/****************************************************************************
** Meta object code from reading C++ file 'Logger_p.h'
**
** Created: Thu Jan 22 20:41:00 2009
**      by: The Qt Meta Object Compiler version 61 (Qt 4.5.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "Logger_p.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Logger_p.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 61
#error "This file was generated using the moc from 4.5.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LoggerPrivate[] = {

 // content:
       2,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   12, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LoggerPrivate[] = {
    "LoggerPrivate\0\0pagerFinished()\0"
};

const QMetaObject LoggerPrivate::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LoggerPrivate,
      qt_meta_data_LoggerPrivate, 0 }
};

const QMetaObject *LoggerPrivate::metaObject() const
{
    return &staticMetaObject;
}

void *LoggerPrivate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LoggerPrivate))
        return static_cast<void*>(const_cast< LoggerPrivate*>(this));
    return QObject::qt_metacast(_clname);
}

int LoggerPrivate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: pagerFinished(); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
