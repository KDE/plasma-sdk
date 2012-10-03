/*
Copyright 2012 Antonis Tsiapaliokas <kok3rs@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "windowswitcherpreviewer.h"
#include <QLabel>
#include <KLocale>


WindowSwitcherPreviewer::WindowSwitcherPreviewer(QWidget* parent)
        : KDialog(parent)
{
    setMinimumSize(400,370);
    tmpWidget = new QWidget();
    tmpLayout = new QVBoxLayout();
    m_previewer = new TabBoxPreviewer(i18nc("Window Title", "Window Switcher Previewer"));
    m_filePath = new KUrlRequester(tmpWidget);
    QLabel *welcome = new QLabel(tmpWidget);

    m_filePath->setFilter("*.qml");
    welcome->setText(i18n("Welcome to the Window Switcher Previewer"));
    m_previewer->hide();

    tmpLayout->addWidget(welcome);
    tmpLayout->addWidget(m_previewer);
    tmpLayout->addWidget(m_filePath);

    connect(m_filePath, SIGNAL(textChanged(const QString&)), this, SLOT(loadPreviewer(const QString&)));

    tmpWidget->setLayout(tmpLayout);
    setMainWidget(tmpWidget);
}

void WindowSwitcherPreviewer::loadPreviewer(const QString& filePath)
{
    m_previewer->show();
    m_previewer->setLayoutPath(m_filePath->text());
}

#include "moc_windowswitcherpreviewer.cpp"