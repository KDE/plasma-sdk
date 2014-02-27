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

#ifndef METADATAHANDLER_H
#define METADATAHANDLER_H

#include <QObject>

#include <KConfigGroup>

class MetadataHandler : public QObject
{
    Q_OBJECT

public:
    MetadataHandler(QObject *parent = 0);

    ~MetadataHandler();

    void setFilePath(const QString &filePath);
    QString filePath() const;

    void writeFile();

    QString name() const;
    void setName(const QString &name);

    QString description() const;
    void setDescription(const QString &description);

    QString icon() const;
    void setIcon(const QString &icon);

    QString category() const;
    void setCategory(const QString & category);

    QString version() const;
    void setVersion(const QString &version);

    QString website() const;
    void setWebsite(const QString &website);

    QString author() const;
    void setAuthor(const QString &author);

    QString email() const;
    void setEmail(const QString &email);

    QString license() const;
    void setLicense(const QString &license);

    QStringList serviceTypes() const;
    void setServiceTypes(const QStringList &serviceTypes);

    QString pluginName() const;
    void setPluginName(const QString &pluginName);

private:
    KConfigGroup m_metadataConfig;
    QString m_filePath;
};

#endif

