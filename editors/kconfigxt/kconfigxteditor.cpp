/*
   This file is part of the KDE project
   Copyright (C) 2009 by Dmitry Suzdalev <dimsuz@gmail.com>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "kconfigxteditor.h"
#include "ui_kconfigxteditor.h"

#include <KDebug>
#include <QFile>

KConfigXtEditor::KConfigXtEditor(QWidget *parent)
    : QWidget(parent)
{
    m_ui = new Ui::KConfigXtEditor;
    m_ui->setupUi(this);
}

KConfigXtEditor::~KConfigXtEditor()
{
    delete m_ui;
}

void KConfigXtEditor::setFilename(const QString& filename)
{
    m_filename = filename;
}

void KConfigXtEditor::readFile()
{
    if (m_filename.isEmpty()) {
        kDebug() << "Empty filename given!";
        return;
    }

    if (!QFile::exists(m_filename)) {
        setupWidgetsForNewFile();
        return;
    }

    // TODO: reading goes here
}

void KConfigXtEditor::writeFile()
{
    // TODO: writing goes here
}

void KConfigXtEditor::setupWidgetsForNewFile()
{

}
