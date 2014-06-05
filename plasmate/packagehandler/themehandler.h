/*

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

#ifndef THEMESHANDLER_H
#define THEMESHANDLER_H

#include "packagehandler.h"

#include <QHash>

class ThemeHandler : public PackageHandler
{
    Q_OBJECT

public:
    explicit ThemeHandler(QObject *parent = 0);
    ~ThemeHandler();

      void createPackage(const QString &userName, const QString &userEmail,
                         const QString &serviceType, const QString &pluginName) override;

    QString description(const QString &entry) const override;

protected:
    QString packageType() const override;

private:
    QHash<QString, QString> m_orphanDirs;
    void createDirectories();
};

#endif

