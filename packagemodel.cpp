/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Artur Duque de Souza <asouza@kde.org>
Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>
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

#include <packagemodel.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

#include <KConfigGroup>
#include <KDesktopFile>
#include <KDirWatch>
#include <KIcon>
#include <KUser>

#include <Plasma/Package>
#include <Plasma/PackageMetadata>

PackageModel::PackageModel(QObject *parent)
        : QAbstractItemModel(parent),
        m_directory(0),
        m_structure(0),
        m_package(0)
{
}

PackageModel::~PackageModel()
{
    delete m_directory;
    delete m_package;
    m_structure = 0;
}

void PackageModel::setPackageType(const QString &type)
{
    m_structure = 0;
    m_structure = Plasma::PackageStructure::load(type);
}

QString PackageModel::packageType() const
{
    if (m_structure) {
        return m_structure->type();
    }

    return QString();
}

QString PackageModel::contentsPrefix() const
{
    return m_structure->contentsPrefix();
}

QString PackageModel::implementationApi() const
{
    if (m_structure) {
        return const_cast<Plasma::PackageStructure::Ptr &>(m_structure)->metadata().implementationApi();
    }

    return QString();
}

int PackageModel::setPackage(const QString &path)
{
    if (!m_structure) {
        kDebug() << "Must set the package type FIRST!";
        return 0;
    }

    m_structure->setPath(path);

    delete m_package;
    m_package = new Plasma::Package(path, m_structure);

    delete m_directory;
    m_directory = new KDirWatch(this);
    m_directory->addDir(path, KDirWatch::WatchSubDirs | KDirWatch::WatchFiles);

    if (!loadPackage())
        return 0;

    connect(m_directory, SIGNAL(created(QString)), this, SLOT(fileAddedOnDisk(QString)));
    connect(m_directory, SIGNAL(deleted(QString)), this, SLOT(fileDeletedOnDisk(QString)));

    return 1;
}

QString PackageModel::package() const
{
    if (m_package) {
        return m_package->path();
    }

    return QString();
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
    if (!m_package) {
        return QVariant();
    }

    const char *key = static_cast<const char *>(index.internalPointer());
    if (key) { // it's a child item
        switch (role) {
        case MimeTypeRole: {
            if (index.row() == 0)
                return QStringList("[plasmate]/new");
            return m_package->structure()->mimetypes(key);
        }
        break;
        case UrlRole: {
            if (index.row() < 0)
                break;
            QList<const char *> named = m_namedFiles.value(key);
            int row = index.row() - 1;
            QString path = "", file = "";
            if (row < 0) { // 'New' entry, return the directory path
                path = m_package->filePath(key);
            } else if (row < named.count()) {
                path = m_package->path();
                QString contents = m_package->structure()->contentsPrefix();
                path = path.endsWith("/") ? path + contents : path + "/" + contents;
                file = m_package->structure()->path(named.at(row));
            } else {
                row -= named.count();
                QStringList l = m_files.value(key);
                if (row < l.count()) {
                    path = m_package->filePath(key);
                    file = l.at(row);
                }
            }
            path = path.endsWith("/") ? path : path + "/";
            return path + file;
        }
        break;
        case Qt::DisplayRole: {
            if (index.row() == 0)
              return i18n("New...");
            QList<const char *> named = m_namedFiles.value(key);
            int row = index.row() - 1;
            if (row < named.count()) {
                //kDebug() << m_package->structure()->name(named.at(row));
                return m_package->structure()->name(named.at(row));
            }
            row -= named.count();
            QStringList l = m_files.value(key);
            if (row < l.count()) {
                //kDebug() << "got" << l.at(index.row() - 1);
                return l.at(row);
            }
        }
        break;
        case Qt::DecorationRole: {
            if (index.row() == 0)
                return KIcon("document-new");
        }
        break;
        }
    } else { // it's a top level item
        switch (role) {
        case Qt::DisplayRole: {
            if (index.row() == m_topEntries.count()) {
                return i18n("Metadata");
            }
            return m_structure->name(m_topEntries.at(index.row()));
        }
        break;
        case MimeTypeRole: {
            if (index.row() == m_topEntries.count()) {
                // not sure if this is good, but will do for now
                // use special wildcard to indicate stuff that
                // plasmate should handle in it's own way
                return QStringList("[plasmate]/metadata");
            }
        }
        break;
        case UrlRole: {
            if (index.row() == m_topEntries.count())
                return m_package->path() + "metadata.desktop";
        }
        break;
        }
    }
    return QVariant();
}

QModelIndex PackageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.row() >= m_topEntries.count() || parent.parent().isValid()) {
            //kDebug() << "FAIL" << row << column;
            return QModelIndex();
        }

        const char *key = m_topEntries.at(parent.row());

        if (row <= m_files[key].count() + m_namedFiles[key].count()) {
            //kDebug() << "going to return" << row << column << key;
            return createIndex(row, column, (void*)key);
        } else {
            //kDebug() << "FAIL";
            return QModelIndex();
        }
    }

    if (row <= m_topEntries.count()) {
        return createIndex(row, column);
    }

    return QModelIndex();
}

QModelIndex PackageModel::parent(const QModelIndex &index) const
{
    const char *key = static_cast<const char *>(index.internalPointer());

    if (m_topEntries.contains(key)) {
        return createIndex(m_topEntries.indexOf(key), 0);
    }

    return QModelIndex();
}

int PackageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    //FIXME: need to accomodate more information
    return 1;
}

int PackageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.row() < m_topEntries.count()) {
            //const char *key = m_topEntries.at(parent.row());
            const char *key = static_cast<const char *>(parent.internalPointer());
            if (!key) {
                key = m_topEntries.at(parent.row());
            //It is a leaf
            } else if (m_namedFiles.contains(key)) {
                return 0;
            }
            //kDebug() << "looking for" << key << m_files[key].count() << m_namedFiles[key]<<key<<parent.parent();
            
            return m_files.contains(key) ? m_files[key].count() + m_namedFiles[key].count() + 1 : 0;
        } else {
            return 0;
        }
    }

    return m_topEntries.count() + 1;
}

int PackageModel::loadPackage()
{
    reset();

    if (!m_package) {
        kDebug() << "No package to load.";
        return 0;
    }

    QDir dir(m_package->path());
    Plasma::PackageStructure::Ptr structure = m_package->structure();

    if (!dir.exists(structure->contentsPrefix()))
    {
        kDebug() << "This is not a valid plasmoid package.";
        return 0;
    }

    if (!dir.exists("metadata.desktop")) {
        KUser user;
        Plasma::PackageMetadata metadata;
        metadata.setAuthor(user.property(KUser::FullName).toString());
        metadata.setLicense("GPL");
        metadata.setName(dir.dirName());
        metadata.setServiceType(structure->type());
        metadata.write(dir.path() + "/metadata.desktop");
    }

    QString contents = structure->contentsPrefix();

    if (!contents.isEmpty()) {
        dir.mkpath(contents);
        dir.cd(contents);
    }

    foreach(const char *key, structure->directories()) {
        QString path = structure->path(key);
        if (!dir.exists(path)) {
            dir.mkpath(path);
        }

        m_topEntries.append(key);
    }

    QList<const char *> files = structure->files();
    QHash<QString, const char *> indexedFiles;
    foreach(const char *key, structure->files()) {
        QString path = structure->path(key);
        if (!dir.exists(path)) {
            QFileInfo info(dir.path() + '/' + path);
            dir.mkpath(dir.relativeFilePath(info.absolutePath()));
            // Create all files, but 'code/main', as this is replaced by a template
            if (path != "code/main") {
                QFile f(dir.path() + '/' + path);
                f.open(QIODevice::WriteOnly);
            }
        }

        indexedFiles.insert(path, key);
    }

    // we don't -1 because we have a "ghost" metadata entry
    beginInsertRows(QModelIndex(), 0, m_topEntries.count());
    endInsertRows();

    foreach (const char *key, structure->directories()) {
        QString path = structure->path(key);
        if (!path.endsWith('/')) {
            path += '/';
        }

        QStringList files = m_package->entryList(key);
        QList<const char *> namedFiles;
        QStringList userFiles;
        foreach(const QString &file, files) {
            QString filePath = path + file;
            if (indexedFiles.contains(filePath)) {
                namedFiles.append(indexedFiles.value(filePath));
                indexedFiles.remove(filePath);
            } else {
                userFiles.append(file);
            }
        }
        m_files[key] = userFiles;
        m_namedFiles[key] = namedFiles;

        kDebug() << m_topEntries.indexOf(key) << key << "has" << files.count() << "files" << files;
        beginInsertRows(createIndex(m_topEntries.indexOf(key), 0), 0, files.count());
        endInsertRows();
    }

    if (!indexedFiles.empty()) {
        int currentTopCount = m_topEntries.count();
        foreach(const char *key, indexedFiles) {
            m_topEntries.append(key);
        }
        kDebug() << "counts:" << currentTopCount << indexedFiles.count();
        beginInsertRows(QModelIndex(), currentTopCount, indexedFiles.count());
        endInsertRows();
    }

    return 1;
}

void PackageModel::fileAddedOnDisk(const QString &path)
{
    if (QFileInfo(path).fileName().at(0) == QChar('.')) {
        return; // we ignore hidden files
    }
    KUrl toAdd(path);
    int parentCount = rowCount(QModelIndex());
    for (int i=0; i < parentCount-1; i++) {
        const char *key = m_topEntries.at(i);
        QList<const char *> named = m_namedFiles.value(key);
        KUrl target(m_package->filePath(key));
        if (target.equals(toAdd.directory())) {
            QModelIndex parent = index(i, 0, QModelIndex());
            int ind = rowCount(parent);
            for (int ii=0; ii<ind; ii++) {
                QModelIndex child = index(ii, 0, parent);
                KUrl childPath(child.data(PackageModel::UrlRole).toString());
                if (childPath.equals(toAdd)) // let's not double-add
                    return;
            }
            m_files[key].append(toAdd.fileName());
            beginInsertRows(parent, ind, ind);
            endInsertRows();
            break;
        }
    }
}

void PackageModel::fileDeletedOnDisk(const QString &path)
{
    if (QFile::exists(path)) // KDirWatch seems overparanoid, so we quickcheck
        return;

    // Probably not the most efficient way to do it but
    // it works :)
    KUrl toDelete(path);

    // Iterate through every tree element and check if it matches
    // the deleted file
    int parentCount = rowCount(QModelIndex());
    for (int i=0; i < parentCount; i++) {
        QModelIndex parent = index(i, 0, QModelIndex());
        int childCount = rowCount(parent);
        for (int ii=1; ii < childCount; ii++) {
            QModelIndex child = index(ii, 0, parent);
            KUrl childPath(child.data(PackageModel::UrlRole).toString());
            if (childPath.equals(toDelete)) {
                // match!! remove it!
                beginRemoveRows(parent, ii, ii);
                endRemoveRows();
                const char *key = m_topEntries.at(i);
                QList<const char *> &named = m_namedFiles[key];
                int row = ii-1;
                if (row < named.count()) {
                    named.removeAt(row);
                } else {
                    row -= named.count();
                    QStringList &l = m_files[key];
                    if (row < l.count()) {
                        l.removeAt(row);
                    }
                }
                break;
            }
        }
    }
}

