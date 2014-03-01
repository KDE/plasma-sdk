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
#include <QFileDialog>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMenuBar>
#include <QTest>

#include <KUrlRequester>
#include <QDebug>

EditPageTest::EditPageTest(QWidget *parent)
        : QMainWindow(parent),
          m_editPage(0),
          m_packageHandler(0),
          m_packageModel(0),
          m_urlRequester(0),
          m_loadButton(0),
          m_currentWidget(0)
{
    m_packageHandler = new PackageHandler();
    m_packageHandler->setPackageType(QStringLiteral("Plasma/Applet"));

    QWidget *container = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    container->setLayout(layout);

    m_editPage = new EditPage();
    m_editPage->setEnabled(false);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    m_urlRequester = new KUrlRequester();
    m_loadButton = new QPushButton(QStringLiteral("load"), this);

    horizontalLayout->addWidget(m_urlRequester);
    horizontalLayout->addWidget(m_loadButton);

    layout->addWidget(new QLabel(QStringLiteral("Choose the metadata.desktop of a package")));
    layout->addLayout(horizontalLayout);

    QHBoxLayout *mainHorizontalLayout = new QHBoxLayout();
    mainHorizontalLayout->addWidget(m_editPage);

    m_currentWidget = new QWidget();
    mainHorizontalLayout->addWidget(m_currentWidget);

    layout->addLayout(mainHorizontalLayout);

    connect(m_editPage, &EditPage::loadRequiredEditor, [=](QWidget *widget) {
        m_currentWidget->hide();
        mainHorizontalLayout->removeWidget(m_currentWidget);
        delete m_currentWidget;
        m_currentWidget = 0;
        m_currentWidget = widget;
        mainHorizontalLayout->addWidget(m_currentWidget);
        m_currentWidget->show();
    });

    QAction *newFileAction = menuBar()->addAction(QStringLiteral("Create New File"));
    QAction *deleteFileAction = menuBar()->addAction(QStringLiteral("Delete New File"));

    setCentralWidget(container);

    connect(m_loadButton, &QPushButton::pressed, [=]() {
        const QString path = m_urlRequester->url().toLocalFile();

        if (path.isEmpty()) {
            return;
        }

        QDir dir(path);
        dir.cdUp();

        m_packageHandler->setPackagePath(dir.path());
        m_editPage->setEnabled(true);

        m_destinationPath = m_packageHandler->packagePath() + m_packageHandler->contentsPrefix();
        // FIXME
        m_destinationPath += QStringLiteral("ui/");

        m_packageModel = new PackageModel(m_packageHandler);

        m_editPage->setModel(m_packageModel);
    });

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
    qApp->quit();
}

PLASMATE_TEST_MAIN(EditPageTest)

