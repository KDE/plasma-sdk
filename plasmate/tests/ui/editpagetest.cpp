/*

Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common.h"
#include "editpagetest.h"

#include "../../packagemodel.h"

#include <QFile>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QTest>

#include <QDebug>

EditPageTest::EditPageTest(QWidget *parent)
        : QMainWindow(parent),
          m_editPage(0),
          m_packageHandler(0),
          m_currentWidget(0)
{
    m_packageHandler = new PackageHandler();
    m_packageHandler->setPackageType(QStringLiteral("Plasma/Applet"));

    m_editPage = new EditPage(m_packageHandler, this);

    QHBoxLayout *l = new QHBoxLayout();
    l->addWidget(m_editPage);

    m_currentWidget = new QWidget();
    l->addWidget(m_currentWidget);


    QWidget *container = new QWidget();
    container->setLayout(l);
    setCentralWidget(container);

    connect(m_editPage, &EditPage::loadRequiredEditor, [=](QWidget *widget) {
        m_currentWidget->hide();
        l->removeWidget(m_currentWidget);
        delete m_currentWidget;
        m_currentWidget = 0;
        m_currentWidget = widget;
        l->addWidget(m_currentWidget);
        m_currentWidget->show();
    });

    QAction *newFileAction = menuBar()->addAction(QStringLiteral("Create New File"));
    QAction *deleteFileAction = menuBar()->addAction(QStringLiteral("Delete New File"));

    m_destinationPath = TEST_DATA + QStringLiteral("org.kde.tests.packagehandertest");
    m_packageHandler->setPackagePath(m_destinationPath);
    m_destinationPath = m_packageHandler->contentsPrefix() + QStringLiteral("ui");

    connect(newFileAction, &QAction::triggered, [=](bool checked) {
        Q_UNUSED(checked);
        QFile f(m_destinationPath + QStringLiteral("testKDirWatch.qml"));
        f.open(QIODevice::ReadWrite);
    });

    connect(deleteFileAction, &QAction::triggered, [=](bool checked) {
        Q_UNUSED(checked);
        QFile f(m_destinationPath + QStringLiteral("testKDirWatch.qml"));
        f.remove();
    });
}

EditPageTest::~EditPageTest()
{
}

PLASMATE_TEST_MAIN(EditPageTest)

