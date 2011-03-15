/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Artur Duque de Souza <asouza@kde.org>
Copyright 2009-2010 Laurent Montel <montel@kde.org>
Copyright 2009-2010 Shantanu Tushar Jha <jhahoneyk@gmail.com>
Copyright 2009-2010 Sandro Andrade <sandroandrade@kde.org>
Copyright 2009-2010 Lim Yuen Hoe <yuenhoe@hotmail.com>

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
#include "editpage.h"

#include <QHBoxLayout>
#include <QFile>
#include <KIcon>
#include <KMessageBox>
#include <KInputDialog>
#include <kio/jobclasses.h>
#include <KIO/MimetypeJob>
#include <KIO/Job>
#include <kmimetypetrader.h>
#include "packagemodel.h"

EditPage::EditPage(QWidget *parent)
        : QTreeView(parent)
{
    setHeaderHidden(true);
    m_contextMenu = new KMenu(this);
    QAction *del = m_contextMenu->addAction(KIcon("window-close"), i18n("Delete"));
    connect(del, SIGNAL(triggered(bool)), this, SLOT(doDelete(bool)));

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(findEditor(const QModelIndex &)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showTreeContextMenu(const QPoint&)));
}

void EditPage::doDelete(bool)
{
    QModelIndexList items = selectedIndexes();
    if (items.empty())
        return; // this shouldn't happen
    QModelIndex selected = items.at(0); // only one can be selected
    QString path = selected.data(PackageModel::UrlRole).toString();
    QString name = selected.data(Qt::DisplayRole).toString();
    QString dialogText = i18n("Are you sure you want to delete this file: ");
    int code = KMessageBox::warningContinueCancel(this, 
                  dialogText + "'" + name + "'?");
    if (code == KMessageBox::Continue)
        QFile::remove(path);
}

void EditPage::showTreeContextMenu(const QPoint&)
{
    QModelIndexList items = selectedIndexes();
    if (items.empty())
        return;

    const char *key = static_cast<const char *>
                              (items.at(0).internalPointer());
    if (!key || // top level item
          items.at(0).row() == 0) // 'New' entry
        return;

    m_contextMenu->popup(QCursor::pos());
}

void EditPage::findEditor(const QModelIndex &index)
{
    QStringList mimetypes = index.data(PackageModel::MimeTypeRole).toStringList();
    foreach(const QString &mimetype, mimetypes) {
        QString target = index.data(PackageModel::UrlRole).toString();
        if (mimetype == "[plasmate]/metadata") {
            emit loadMetaDataEditor(target);
            return;
        }
        if (mimetype == "[plasmate]/new") {
            QString dialogText = i18n( "Enter a name for the new file:" );
            QString file = KInputDialog::getText(QString(), dialogText);
            if (!file.isEmpty()) {
                file = target + file;
                QFile fl(file);
                fl.open(QIODevice::ReadWrite); // create the file
                fl.close();
            }
        }
        KService::List offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadWritePart");
        kDebug() << mimetype;
        if (offers.isEmpty()) {
            offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadOnlyPart");
        }
        if (!offers.isEmpty()) {
            //create the part using offers.at(0)
            //kDebug() << offers.at(0);
            //offers.at(0)->createInstance(parentWidget);
            emit loadEditor(offers, KUrl(target));
            return;
        }
    }
}

void EditPage::loadFile(const KUrl &path, const QString &mimetype)
{
    kDebug() << "Loading file: " << path << mimetype;
    // mimetype?
    QString _mimetype;

    if (mimetype.isEmpty()) {
        KIO::JobFlags flags = KIO::HideProgressInfo;
        KIO::MimetypeJob *mjob = KIO::mimetype(path, flags);
        mjob->exec();

        if (mjob->error()) {
            kWarning() << "Error retrieving mimetype from:" << path << ": " << mjob->errorText();
            return;
        }
        _mimetype = mjob->mimetype();
        kDebug() << "loaded mimetype:" << _mimetype;
    } else {
        _mimetype = mimetype;
    }

    KService::List offers = KMimeTypeTrader::self()->query(_mimetype, "KParts/ReadWritePart");
    kDebug() << _mimetype;
    if (offers.isEmpty()) {
        offers = KMimeTypeTrader::self()->query(_mimetype, "KParts/ReadOnlyPart");
    }
    if (!offers.isEmpty()) {
        //create the part using offers.at(0)
        //kDebug() << offers.at(0);
        //offers.at(0)->createInstance(parentWidget);
        emit loadEditor(offers, path);
        return;
    }
    kDebug() << "loading" << path;
}