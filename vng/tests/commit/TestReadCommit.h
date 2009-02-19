#ifndef TESTREADCOMMITS_H
#define TESTREADCOMMITS_H

#include <QObject>
#include <QtTest/QtTest>

class TestReadCommit : public QObject {
    Q_OBJECT
public:
    TestReadCommit() {}

private slots:
    void testBasis();
    void testCommit();
    void readFromCatFile();
    void twoParents();
    void justFiles();
    void multipleCommits();
};

#endif
