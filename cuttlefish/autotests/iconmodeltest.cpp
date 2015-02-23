/*
 *  Copyright 2014 Alex Richardson <arichardson.kde@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License version 2 as published by the Free Software Foundation;
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include <QtTest/QTest>

#include <QDebug>
#include <QJsonDocument>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QSignalSpy>

#include "iconmodel.h"


using namespace CuttleFish;

class IconModelTest : public QObject
{
    Q_OBJECT


private Q_SLOTS:

    void init()
    {
        m_iconModel = new IconModel(this);
    }

    void cleanup()
    {
        delete m_iconModel;
    }

    void initTestCase()
    {
    };

    void testCategoryFilter()
    {
        const int _all = m_iconModel->rowCount(QModelIndex());

        m_iconModel->setFilter("edit");
        const int _edit = m_iconModel->rowCount(QModelIndex());

        m_iconModel->setCategory("actions");
        const int _editactions = m_iconModel->rowCount(QModelIndex());

        m_iconModel->setCategory("all");
        const int _alledit = m_iconModel->rowCount(QModelIndex());

        QVERIFY(_all > _edit);
        QVERIFY(_all > _editactions);
        QVERIFY(_edit >= _editactions);
        QVERIFY(_alledit >= _editactions);
    }



private: // disable from here for testing just the above


private:
    QJsonArray m_data;
    QJsonArray m_empty;

    IconModel* m_iconModel;

};

QTEST_MAIN(IconModelTest)

#include "iconmodeltest.moc"
