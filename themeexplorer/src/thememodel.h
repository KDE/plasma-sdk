/*
 *   SPDX-FileCopyrightText: 2015 Marco Martin <mart@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef THEMEMODEL_H
#define THEMEMODEL_H

#include <KSvg/ImageSet>
#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QQmlEngine>
#include <kpackage/package.h>
#include <qqmlintegration.h>

namespace Plasma
{
class Theme;
}

class ThemeListModel;
class ColorEditor;

class ThemeModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY themeChanged)
    Q_PROPERTY(ThemeListModel *themeList READ themeList CONSTANT)
    Q_PROPERTY(ColorEditor *colorEditor READ colorEditor CONSTANT)

    Q_PROPERTY(QString author READ author NOTIFY themeChanged)
    Q_PROPERTY(QString email READ email NOTIFY themeChanged)
    Q_PROPERTY(QString license READ license NOTIFY themeChanged)
    Q_PROPERTY(QString website READ website NOTIFY themeChanged)
    Q_PROPERTY(bool isWritable READ isWritable NOTIFY isWritableChanged)

    Q_PROPERTY(QString themeFolder READ themeFolder NOTIFY themeChanged)
public:
    enum Roles {
        ImagePath,
        Description,
        Delegate,
        UsesFallback,
        SvgAbsolutePath,
        IconElements,
        FrameSvgPrefixes,
    };

    static ThemeModel *create(QQmlEngine *, QJSEngine *)
    {
        static ThemeModel instance;
        QQmlEngine::setObjectOwnership(&instance, QQmlEngine::CppOwnership);
        return &instance;
    };

    ThemeListModel *themeList();
    ColorEditor *colorEditor();

    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    void setTheme(const QString &theme);
    QString theme() const;

    QString author() const;
    QString email() const;
    QString license() const;
    QString website() const;

    bool isWritable() const;

    void load();

    Q_INVOKABLE void editElement(const QString &imagePath);
    Q_INVOKABLE void editThemeMetaData(const QString &name, const QString &author, const QString &email, const QString &license, const QString &website);
    Q_INVOKABLE void createNewTheme(const QString &name, const QString &author, const QString &email, const QString &license, const QString &website);

    QString themeFolder();

Q_SIGNALS:
    void themeChanged();
    void isWritableChanged();

private Q_SLOTS:
    void processFinished();

private:
    QHash<int, QByteArray> m_roleNames;
    explicit ThemeModel();

    Plasma::Theme *m_theme;
    QString m_themeName;
    KSvg::ImageSet m_imageSet;
    QJsonDocument m_jsonDoc;
    ThemeListModel *m_themeListModel;
    ColorEditor *m_colorEditor;
};

#endif // THEMEMODEL_H
