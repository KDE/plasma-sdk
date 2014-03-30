/*

Copyright 2013 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>
Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

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
            virtual ~Node();
            QString name() const;
            QString description() const;
            QStringList mimeTypes() const;
            bool isFile() const;
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

    virtual void setPackagePath(const QString &path);

    QString packagePath() const;

    QString contentsPrefix() const;

    virtual PackageHandler::Node* loadPackageInfo() = 0;

    virtual QUrl urlForNode(PackageHandler::Node *node) = 0;

    virtual void createPackage(const QString &userName, const QString &userEmail,
                               const QString &serviceType, const QString &pluginName);

Q_SIGNALS:
    void error(const QString &errorMessage);
    void packageChanged(PackageHandler::Node* info);

protected:
    Plasma::Package package() const;

private:
    QString m_packagePath;

    Plasma::Package m_package;
    void createRequiredDirectories();
};

#endif

