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
#include <KIcon>
#include <QFile>
#include <QHeaderView>

class KConfigXtEditorPrivate
{
public:
    Ui::KConfigXtEditor *ui;
    QString filename;
    QStringList groups;
};

KConfigXtEditor::KConfigXtEditor(QWidget *parent)
    : QWidget(parent), d(new KConfigXtEditorPrivate)
{
    d->ui = new Ui::KConfigXtEditor;
    d->ui->setupUi(this);

    d->ui->twKeyValues->header()->setResizeMode(QHeaderView::ResizeToContents);
    d->ui->twGroups->header()->setResizeMode(QHeaderView::ResizeToContents);

    d->ui->lblHintIcon->setPixmap(KIcon("dialog-information").pixmap(16,16));

    connect(d->ui->pbAddGroup, SIGNAL(clicked()), SLOT(createNewGroup()));
}

KConfigXtEditor::~KConfigXtEditor()
{
    delete d->ui;
    delete d;
}

void KConfigXtEditor::setFilename(const QString& filename)
{
    d->filename = filename;
}

void KConfigXtEditor::readFile()
{
    if (d->filename.isEmpty()) {
        kDebug() << "Empty filename given!";
        return;
    }

    if (!QFile::exists(d->filename)) {
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
    // Add default group
    createNewGroup();
}

void KConfigXtEditor::createNewGroup()
{
    QString newGroupName;
    if (d->groups.isEmpty()) {
        newGroupName = "General";
    } else {
        int counter = 1;
        newGroupName = QString("Group %1").arg(counter);
        while (d->groups.contains(newGroupName)) {
            counter++;
            newGroupName = QString("Group %1").arg(counter);
        }
    }

    d->groups.append(newGroupName);

    QTreeWidgetItem* item = new QTreeWidgetItem;
    item->setText(0, newGroupName);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    d->ui->twGroups->addTopLevelItem(item);

    d->ui->twGroups->setCurrentItem(item);
    d->ui->twGroups->editItem(item);
}
