/*

Copyright 2009-2010 Aaron Seigo <aseigo@kde.org>
Copyright 2009-2010 Artur Duque de Souza <asouza@kde.org>
Copyright 2009-2010 Diego '[Po]lentino' Casella <polentino911@gmail.com>
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

#include "packagemodel.h"
#include "packagehandler.h"

#include <QIcon>
#include <QUrl>
#include <QMimeDatabase>

#include <KDirWatch>

PackageModel::PackageModel(PackageHandler *packageHandler, QObject *parent)
    : QAbstractItemModel(parent),
      m_packageHandler(packageHandler),
      m_topNode(0),
      m_watchDirectory(new KDirWatch(this))
{
    m_dialogOptions["widgets"] = QStringList("[plasmate]/themeImageDialog/widgets");
    m_dialogOptions["animations"] = QStringList("[plasmate]/themeImageDialog/animations");
    m_dialogOptions["dialogs"] = QStringList("[plasmate]/themeImageDialog/dialogs");
    m_dialogOptions["locolor/dialogs"] = QStringList("[plasmate]/themeImageDialog/locolor/dialogs");
    m_dialogOptions["locolor/widgets"] = QStringList("[plasmate]/themeImageDialog/locolor/widgets");
    m_dialogOptions["opaque/dialogs"] = QStringList("[plasmate]/themeImageDialog/opaque/dialogs");
    m_dialogOptions["opaque/widgets"] = QStringList("[plasmate]/themeImageDialog/opaque/widgets");
    m_dialogOptions["wallpapers"] = QStringList("[plasmate]/themeImageDialog/wallpapers");
    m_dialogOptions["config"] = QStringList("[plasmate]/mainconfigxml/new");
    m_dialogOptions["images"] = QStringList("[plasmate]/imageDialog");

    m_topNode = m_packageHandler->loadPackageInfo();

    m_watchDirectory->addDir(m_packageHandler->packagePath() + m_packageHandler->contentsPrefix(),
                             KDirWatch::WatchSubDirs | KDirWatch::WatchFiles);

    connect(m_watchDirectory, &KDirWatch::created, [=](const QString &path) {
        Q_UNUSED(path);
        beginResetModel();
        m_topNode = m_packageHandler->loadPackageInfo();
        endResetModel();
    });

    connect(m_watchDirectory, &KDirWatch::deleted, [=](const QString &path) {
        Q_UNUSED(path);
        beginResetModel();
        m_topNode = m_packageHandler->loadPackageInfo();
        endResetModel();
    });
}

PackageModel::~PackageModel()
{
}

QModelIndex PackageModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    PackageHandler::Node *parentNode = 0;
    if (!parent.isValid()) {
        parentNode = m_topNode;
    } else {
        parentNode = static_cast<PackageHandler::Node*>(parent.internalPointer());
    }
    PackageHandler::Node *childNode = parentNode->child(row);

    if (childNode) {
        return createIndex(row, column, childNode);
    }
    return QModelIndex();
}

QModelIndex PackageModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    PackageHandler::Node *childNode = static_cast<PackageHandler::Node*>(index.internalPointer());
    PackageHandler::Node *parentNode = childNode->parent();
    if (parentNode == m_topNode) {
        return QModelIndex();
    }

    return createIndex(parentNode->row(), 0, parentNode);
}

int PackageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    PackageHandler::Node *parentNode = 0;
    if (!parent.isValid()) {
        parentNode = m_topNode;
    } else {
        parentNode = static_cast<PackageHandler::Node*>(parent.internalPointer());
    }

    return parentNode->childNodes().size();
}

int PackageModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant PackageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    PackageHandler::Node *node = static_cast<PackageHandler::Node*>(index.internalPointer());

    if (!node) {
        return QVariant();
    }

    switch(role) {
    case MimeTypeRole: {
        return node->mimeTypes();
    }
    break;
    case Qt::DisplayRole: {
        return node->description();
    }
    break;
    case UrlRole: {
        return m_packageHandler->urlForNode(node);
    }
    break;
    case Qt::DecorationRole: {
        if (!node->isFile()) {
            return QVariant();
        }

        if (index.row() == 1) {
            return QIcon::fromTheme(QStringLiteral("document-new"));
        }
        QMimeDatabase db;
        return QIcon::fromTheme(db.mimeTypeForFile(node->name()).iconName());
    }
    }

    return QVariant();
}

