
#include <packagemodel.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>

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

void PackageModel::setPackage(const QString &path)
{
    if (!m_structure) {
        kDebug() << "Must set the package type FIRST!";
        return;
    }

    m_structure->setPath(path);

    delete m_package;
    m_package = new Plasma::Package(path, m_structure);

    delete m_directory;
    m_directory = new KDirWatch(this);
    m_directory->addDir(path, KDirWatch::WatchSubDirs);

    loadPackage();

    connect(m_directory, SIGNAL(created(QString)), this, SLOT(fileAddedOnDisk(QString)));
    connect(m_directory, SIGNAL(deleted(QString)), this, SLOT(fileDeletedOnDisk(QString)));
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

    //TODO: other display roles!
    const char *key = static_cast<const char *>(index.internalPointer());
    if (key) {
        QStringList l = m_files.value(key);
        if (index.row() == 0) {
            if (role == Qt::DisplayRole) {
                return i18n("New");
            } else if (role == Qt::DecorationRole) {
                return KIcon("file-new");
            }
        } else if (index.row() <= l.count()) {
            //kDebug() << "got" << l.at(index.row());
            if (role == Qt::DisplayRole) {
                return l.at(index.row() - 1);
            }
        }
    } else if (role == Qt::DisplayRole) {
        if (index.row() == m_topEntries.count()) {
            return i18n("Metadata");
        }

        return m_structure->name(m_topEntries.at(index.row()));
    }

    return QVariant();
}

QModelIndex PackageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.row() >= m_topEntries.count()) {
            return QModelIndex();
        }

        const char *key = m_topEntries.at(parent.row());

        if (row < m_files[key].count()) {
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
    //FIXME: need to accmodate more information
    return 1;
}

int PackageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.row() < m_topEntries.count()) {
            const char *key = m_topEntries.at(parent.row());
            //kDebug() << "looking for" << key << m_files[key].count();
            return m_files[key].count();
        } else {
            return 0;
        }
    }

    return m_topEntries.count() + 1;
}

void PackageModel::loadPackage()
{
    reset();

    if (!m_package) {
        kDebug() << "No package to load.";
        return;
    }

    QDir dir(m_package->path());
    Plasma::PackageStructure::Ptr structure = m_package->structure();

    if (!dir.exists("metadata.desktop")) {
        KUser user;
        Plasma::PackageMetadata metadata;
        metadata.setAuthor(user.fullName());
        metadata.setLicense("GPL");
        metadata.setName(dir.dirName());
        metadata.setServiceType(structure->type());

        metadata.write(dir.path() + "metadata.desktop");
    }

    QString contents = structure->contentsPrefix();
    if (!contents.isEmpty()) {
        dir.mkpath(contents);
        dir.cd(contents);
    }

    foreach (const char *key, structure->directories()) {
        QString path = structure->path(key);
        if (!dir.exists(path)) {
            dir.mkpath(path);
        }

        m_topEntries.append(key);
    }

    foreach (const char *key, structure->files()) {
        QString path = structure->path(key);
        if (!dir.exists(path)) {
            QFileInfo info(dir.path() + '/' + path);
            dir.mkpath(dir.relativeFilePath(info.absolutePath()));
            QFile f(dir.path() + '/' + path);
            f.open(QIODevice::WriteOnly);
        }

        m_topEntries.append(key);
    }

    // we don't -1 because we have a "ghost" metadata entry
    beginInsertRows(QModelIndex(), 0, m_topEntries.count());
    endInsertRows();

    foreach (const char *key, structure->directories()) {
        QStringList files = m_package->entryList(key);
        m_files[key] = files;

        if (files.isEmpty()) {
            continue;
        }

        //kDebug() << m_topEntries.indexOf(key) << key << "has" << files.count() << "files" << files;
        beginInsertRows(createIndex(m_topEntries.indexOf(key), 0), 0, files.count());
        endInsertRows();
    }
}

void PackageModel::fileAddedOnDisk(const QString &path)
{
    kDebug() << path;
}

void PackageModel::fileDeletedOnDisk(const QString &path)
{
    kDebug() << path;
}

