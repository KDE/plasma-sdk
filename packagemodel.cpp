
#include <packagemodel.h>

#include <KDirWatch>

#include <Plasma/Package>

PackageModel::PackageModel(QObject *parent)
    : QAbstractItemModel(parent),
      m_directory(0),
      m_structure(0),
      m_package(0),
      m_rowCount(0)
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

    QModelIndex parent = index.parent();
    const char *key = static_cast<const char *>(index.internalPointer());
    if (parent.isValid()) {
        QStringList l = m_files[key];
        if (index.row() < l.count()) {
            return l.at(index.row());
        }
    } else if (role == Qt::DisplayRole) {
        return m_structure->name(m_topEntries.at(index.row()));
    }

    return QVariant();
}

QModelIndex PackageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        //FIXME: do some bounds checking on the file list
        const char *key = static_cast<const char *>(parent.internalPointer());

        if (row < m_files[key].count()) {
            return createIndex(row, column, parent.internalPointer());
        } else {
            return QModelIndex();
        }
    }

    if (row < m_topEntries.count()) {
        return createIndex(row, column, (void*)0);
    }

    return QModelIndex();
}

QModelIndex PackageModel::parent(const QModelIndex &index) const
{
    const char *key = static_cast<const char *>(index.internalPointer());

    if (m_topEntries.contains(key)) {
        createIndex(m_topEntries.indexOf(key), 0, (void*)key);
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
        const char *key = static_cast<const char *>(parent.internalPointer());
        return m_files[key].count();
    }

    return m_topEntries.count();
}

void PackageModel::loadPackage()
{
    reset();

    if (!m_package) {
        kDebug() << "No package to load.";
        return;
    }

    foreach (const char *key, m_structure->directories()) {
        m_topEntries.append(key);
    }

    foreach (const char *key, m_structure->files()) {
        m_topEntries.append(key);
    }

    beginInsertRows(QModelIndex(), 0, m_topEntries.count() - 1);
    endInsertRows();

    foreach (const char *key, m_structure->directories()) {
        QStringList files = m_package->entryList(key);
        m_files[key] = files;

        if (files.isEmpty()) {
            continue;
        }

        beginInsertRows(createIndex(m_topEntries.indexOf(key), 0, (void*)key), 0, m_files[key].count());
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

