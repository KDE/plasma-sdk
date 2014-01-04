/*

Copyright 2013 Giorgos Tsiapaliokas <terietor@gmail.com>

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

#ifndef PACKAGEHANDLER_H
#define PACKAGEHANDLER_H

#include <QObject>

#include <Plasma/Package>

class KDirWatch;

class PackageHandler : public QObject
{
    Q_OBJECT

public:
    explicit PackageHandler(QObject *parent = 0);
    ~PackageHandler();

    class Node {
        public:
            Node(const QString &name, const QString &description,
                 const QStringList &mimetypes = QStringList(),
                 PackageHandler::Node *parent = 0);
            ~Node();
            QString name() const;
            QString description() const;
            QStringList mimeTypes() const;
            bool isFile() const;
            bool needsNewFileNode() const;
            QList<PackageHandler::Node*> childNodes() const;
            PackageHandler::Node* parent() const;
            PackageHandler::Node* child(int row) const;
            int row();
            void addChild(Node *child);
        private:
            QString m_name;
            QString m_description;
            QList<PackageHandler::Node*> m_childNodes;
            PackageHandler::Node *m_parent;
            QStringList m_mimeTypes;
    };

    void setPackageType(const QString &type);

    void setPackagePath(const QString &path);

    QString packagePath() const;

    QString contentsPrefix() const;

    PackageHandler::Node* loadPackageInfo();

    QUrl urlForNode(PackageHandler::Node *node) const;

Q_SIGNALS:
    void error(const QString &errorMessage);
    void packageChanged(PackageHandler::Node* info);

private:
    Plasma::Package m_package;
    QString m_packagePath;
    QHash<QString, QString> m_fileDefinitions;
    QMultiHash<QString, QString> m_directoryDefinitions;
    PackageHandler::Node* m_topNode;


    void createPackage();
    void createRequiredDirectories();
    void createRequiredFiles();
};

#endif

