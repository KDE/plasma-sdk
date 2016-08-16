/*
 *   Copyright 2016 Marco Martin <mart@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef LNFLOGIC_H
#define LNFLOGIC_H

#include <QAbstractListModel>
#include <kpackage/package.h>


class LnfListModel;
class QDBusPendingCallWatcher;

class LnfLogic : public QObject
{
    Q_OBJECT

    Q_PROPERTY(LnfListModel *lnfList READ lnfList CONSTANT)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(bool isWritable READ isWritable NOTIFY themeChanged)
    Q_PROPERTY(QString themeFolder READ themeFolder NOTIFY themeChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY themeChanged)
    Q_PROPERTY(QString comment READ comment WRITE setComment NOTIFY themeChanged)
    Q_PROPERTY(QString author READ author WRITE setAuthor NOTIFY themeChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY themeChanged)
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY themeChanged)
    Q_PROPERTY(QString website READ website WRITE setWebsite NOTIFY themeChanged)
    Q_PROPERTY(QString license READ license WRITE setLicense NOTIFY themeChanged)
    Q_PROPERTY(QString thumbnailPath READ thumbnailPath NOTIFY themeChanged)

public:
    enum ErrorLevel {
        Info,
        Error
    };
    Q_ENUMS(ErrorLevel)

    explicit LnfLogic(QObject *parent = 0);
    ~LnfLogic();

    void writeMetadata(const QString &key, const QString &value);

    LnfListModel *lnfList();

    void setTheme(const QString &theme);
    QString theme() const;

    bool isWritable() const;
    QString themeFolder() const;

    QString name() const;
    void setName(const QString &name);

    QString comment() const;
    void setComment(const QString &comment);

    QString author() const;
    void setAuthor(const QString &author);

    QString email() const;
    void setEmail(const QString &email);

    QString version() const;
    void setVersion(const QString &version);

    QString website() const;
    void setWebsite(const QString &website);

    QString license() const;
    void setLicense(const QString &license);

    QString thumbnailPath() const;

    void dumpPlasmaLayout(const QString &pluginName);

    Q_INVOKABLE void createNewTheme(const QString &pluginName, const QString &name, const QString &comment, const QString &author, const QString &email, const QString &license, const QString &website);
    Q_INVOKABLE void processThumbnail(const QString &path);
    Q_INVOKABLE QString openFile();

    Q_INVOKABLE void dumpCurrentPlasmaLayout();
    Q_INVOKABLE void dumpDefaultsConfigFile(const QString &pluginName);

Q_SIGNALS:
    void themeChanged();
    void messageRequested(ErrorLevel level, const QString &message);

private:
    QString m_themeName;
    KPackage::Package m_package;
    LnfListModel *m_lnfListModel;
};

#endif // LNFLOGIC_H
