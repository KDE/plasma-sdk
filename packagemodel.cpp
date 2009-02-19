
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
}

QModelIndex PackageModel::index(int row, int column, const QModelIndex &parent) const
{
}

QModelIndex PackageModel::parent(const QModelIndex &index) const
{
}

int PackageModel::columnCount(const QModelIndex &parent) const
{
    //FIXME: need to accmodate more information
    return 1;
}

int PackageModel::rowCount(const QModelIndex & parent) const
{
    return m_rowCount;
}

void PackageModel::loadPackage()
{
    reset();

    if (!m_package) {
        kDebug() << "No package to load.";
        return;
    }

    foreach (const char *key, m_structure->directories()) {
        
    }

    foreach (const char *key, m_structure->files()) {

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

