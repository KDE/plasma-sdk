
#ifndef PACKAGEMODEL_H
#define PACKAGEMODEL_H

#include <QAbstractItemModel>

#include <Plasma/PackageStructure>

class KDirWatch;

namespace Plasma
{
    class Package;
} // namespace Plasma

class PackageModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit PackageModel(QObject *parent = 0);
    ~PackageModel();

    void setPackageType(const QString &type);
    QString packageType() const;

    void setPackage(const QString &path);
    QString package() const;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;

private:
    void loadPackage();

private Q_SLOTS:
    void fileAddedOnDisk(const QString &path);
    void fileDeletedOnDisk(const QString &path);

private:
    KDirWatch *m_directory;
    Plasma::PackageStructure::Ptr m_structure;
    Plasma::Package *m_package;
    uint m_rowCount;
};

#endif

