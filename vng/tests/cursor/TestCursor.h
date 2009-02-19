#ifndef TESTCURSOR_H
#define TESTCURSOR_H

#include <QObject>
#include <QtTest/QtTest>
#include "hunks/ChangeSet.h"

class TestCursor : public QObject {
    Q_OBJECT
public:
    TestCursor() {}

private slots:
    void testBasis();
    void testForward1();
    void testForward2();
    void testBackwards();
    void testPartialSelection();

private:
    ChangeSet createChangeSet();
};

#endif
