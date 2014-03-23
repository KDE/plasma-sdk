/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Artur Duque de Souza <asouza@kde.org>
Copyright 2009-2010 Laurent Montel <montel@kde.org>
Copyright 2009-2010 Shantanu Tushar Jha <shaan7in@gmail.com>
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
#include "imageviewer/imageviewer.h"
#include "kconfigxt/kconfigxteditor.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QFile>
#include <QList>
#include <QMenu>
#include <QPixmap>
#include <QStringList>
#include <KConfigGroup>
#include <QFileDialog>
#include <QIcon>
#include <KMessageBox>
#include <QInputDialog>
#include <KLocalizedString>
#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KIO/MimetypeJob>
#include <KIO/Job>
#include <KUser>
#include <kmimetypetrader.h>

#include "../packagemodel.h"

#include <qvarlengtharray.h>

FileList::FileList(PackageHandler *packageHandler, QWidget *parent)
        : QDockWidget(i18n("Files"), parent),
          m_editPage(0)
{
    setObjectName(QStringLiteral("Files"));
    m_editPage = new EditPage(packageHandler, this);
    setWidget(m_editPage);
}

EditPage* FileList::editPage() const
{
    return m_editPage;
}



EditPage::EditPage(PackageHandler *packageHandler, QWidget *parent)
        : QTreeView(parent),
          m_imageViewer(0),
          m_kconfigXtEditor(0),
          m_packageModel(0),
          m_packageHandler(packageHandler)
        // FIXME
        // m_metaEditor(0)
{
    setHeaderHidden(true);
    m_contextMenu = new QMenu(this);
    QAction *del = m_contextMenu->addAction(QIcon::fromTheme("window-close"), i18n("Delete"));
    connect(del, SIGNAL(triggered(bool)), this, SLOT(doDelete(bool)));

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(activated(const QModelIndex &)), this, SLOT(findEditor(const QModelIndex &)));
    connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
        this, SLOT(showTreeContextMenu(const QPoint&)));

    m_packageModel = new PackageModel(m_packageHandler, this);
    setModel(m_packageModel);
}

void EditPage::doDelete(bool)
{
    QModelIndexList items = selectedIndexes();
    if (items.empty()) {
        return; // this shouldn't happen
    }
    QModelIndex selected = items.at(0); // only one can be selected
    const QString path = selected.data(PackageModel::UrlRole).toString();
    const QString name = selected.data(Qt::DisplayRole).toString();
    const QString dialogText = i18n("Are you sure you want to delete the file \"%1\"?", name);
    int code = KMessageBox::warningContinueCancel(this, dialogText);
    if (code == KMessageBox::Continue) {
        QFile::remove(path);

        QDir dirPath(path);
        //our path points to a file, so go up one dir
        dirPath.cdUp();
        foreach(const QFileInfo& fileInfo, dirPath.entryInfoList(QDir::AllEntries)) {
            if (fileInfo.isFile()) {
                //if there is no file the foreach
                //will end and the directory will be deleted
                return;
            }
        }
        KIO::del(QUrl::fromLocalFile(dirPath.path()));
    }
}

void EditPage::showTreeContextMenu(const QPoint&)
{
    QModelIndexList items = selectedIndexes();
    if (items.empty()) {
        return;
    }

    const QStringList mimeTypeList = items.at(0).data(PackageModel::MimeTypeRole).toStringList();
    const QStringList pathList = items.at(0).data(PackageModel::UrlRole).toStringList();

    if (mimeTypeList.isEmpty() ||
        pathList.isEmpty() ||
        mimeTypeList.at(0).startsWith("[plasmate]/") ||
        !QFile::exists(pathList.at(0))) {
        return;
    }

    m_contextMenu->popup(QCursor::pos());
}

void EditPage::findEditor(const QModelIndex &index)
{
    const QStringList mimetypes = index.data(PackageModel::MimeTypeRole).toStringList();
    const QString packagePath = index.data(PackageModel::PackagePathRole).toString();
    const QString contentWithSubdir = index.data(PackageModel::ContentsWithSubdirRole).toString();

    foreach (const QString &mimetype, mimetypes) {
        QString target = index.data(PackageModel::UrlRole).toUrl().toLocalFile();
        qDebug() << "mimetype" << mimetype;
        if (mimetype == "[plasmate]/metadata") {
            emit loadMetaDataEditor(QUrl::fromLocalFile(target));
            return;
        }

        if (mimetype == "[plasmate]/imageDialog") {
            imageDialog("*.png *.gif *.svg *.jpeg *.svgz", "contents/images/");
            return;
        }

        QString commonFilter = "*.svg *.svgz";

        const QString themeType = "[plasmate]/themeImageDialog/";
        if (mimetype.startsWith(themeType)) {
            const QString opts = "contents/" + mimetype.right(mimetype.size() - themeType.length());
            imageDialog(commonFilter, opts);
            return;
        }

        if (mimetype == "[plasmate]/imageViewer") {
            if (!m_imageViewer) {
                m_imageViewer = new ImageViewer(this);
            }

            m_imageViewer->loadImage(QUrl::fromLocalFile(target));
            emit loadRequiredEditor(m_imageViewer);
            return;
        }

        if (mimetype == "[plasmate]/kconfigxteditor") {
            if (!m_kconfigXtEditor) {
                m_kconfigXtEditor = new KConfigXtEditor(this);
            }

            m_kconfigXtEditor->setFilename(QUrl::fromLocalFile(target));
            m_kconfigXtEditor->readFile();

            emit loadRequiredEditor(m_kconfigXtEditor);
            return;
        }

        if (mimetype == "[plasmate]/kcolorscheme") {
            QFile f(target);
            f.open(QIODevice::ReadWrite); // create the file
            return;
        }

        if (mimetype == "[plasmate]/new") {
            const QString dialogText = i18n("Enter a name for the new file:");
            QString file = QInputDialog::getText(this, i18n("Choose a filename"),
                                                 i18n("Enter a name for the new file:"));
            if (!file.isEmpty()) {
                /*if (!m_metaEditor) {
                    m_metaEditor = new MetaDataEditor(this);
                    m_metaEditor->setFilename(packagePath + "/metadata.desktop");
                }*/

                // FIXME
                // if (m_metaEditor->isValidMetaData()) {
                if (true) {
                    // FIXME
                    // const QString api = m_metaEditor->api();
                    const QString api = QStringLiteral("declarativeappletscript");

                    // FIXME
                    // we don't need the m_metaEditor anymore
                    // delete m_metaEditor;
                    // m_metaEditor = 0;

                    if (!api.isEmpty()) {
                        if (!hasExtension(file)) {
                            if (api =="Javascript" && !file.endsWith(".js")) {
                                file.append(".js");
                            } else if (api =="declarativeappletscript" && !file.endsWith(".qml")) {
                                file.append(".qml");
                            }
                        }
                    }


                    file = target + file;
                    QFile fl(file);
                    fl.open(QIODevice::ReadWrite); // create the file
                    fl.close();
                }

            }

            return;
        }

        KService::List offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadWritePart");
        //qDebug() << mimetype;
        if (offers.isEmpty()) {
            offers = KMimeTypeTrader::self()->query(mimetype, "KParts/ReadOnlyPart");
        }

        if (!offers.isEmpty()) {
            //create the part using offers.at(0)
            //qDebug() << offers.at(0);
            //offers.at(0)->createInstance(parentWidget);
            emit loadTextEditor(QUrl::fromLocalFile(target));
            return;
        }
    }
}
void EditPage::imageDialog(const QString& filter, const QString& destinationPath)
{
    KUser user;
    QUrl homeDir(user.homeDir());
    const QList<QUrl> srcDir = QFileDialog::getOpenFileUrls(this, filter, homeDir);
    KConfigGroup cg(KSharedConfig::openConfig(), "PackageModel::package");
    const QUrl destinationDir(cg.readEntry("lastLoadedPackage", QString()) + destinationPath);
    QDir destPath(destinationDir.path());
    if (!destPath.exists()) {
        destPath.mkpath(destinationDir.path());
    }

    if (!srcDir.isEmpty()) {
        foreach(const QUrl source, srcDir) {
            KIO::copy(source, destinationDir, KIO::HideProgressInfo);
        }
    }
}

bool EditPage::hasExtension(const QString& filename)
{
    QStringList list;
    list  << ".js" << ".qml" << ".xml";
    foreach (const QString &str, list) {
        if (filename.endsWith(str)) {
            return true;
        }
    }
    return false;
}

