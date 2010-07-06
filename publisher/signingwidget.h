/*
 *  Copyright (C) 2010 by Diego '[Po]lentino' Casella <polentino911@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software Foundation
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef SIGNINGWIDGET_H
#define SIGNINGWIDGET_H

#include <QtCrypto/QtCrypto>
#include <QWidget>
#include <QVector>

class QTreeWidget;
class QPushButton;
class QStringList;
class QCheckBox;
class Kurl;

class SigningWidget : public QWidget
{
    Q_OBJECT

public:
    SigningWidget();
    bool signingEnabled() const;
    bool sign(const KUrl &path) const;

public Q_SLOTS:
    void setEnabled(const bool enabled);

private:
    void loadConfig();
    void initUI();
    void initKeys();

    QTreeWidget *m_treeWidget;
    QPushButton *m_createKeyButton;
    QPushButton *m_deleteKeyButton;
    QCheckBox* m_signCheckBox;
    bool m_signingEnabled;
    QStringList m_strings;
    QString m_currentKey;
    QCA::Initializer init;
    QCA::KeyStoreManager m_keyStoreManager;
    QCA::KeyStore *m_userKeyStore;

private Q_SLOTS:
    void createKey();
    void deleteKey();
    void loadKeys();
    void updateCurrentKey();
};

#endif // SIGNINGWIDGET_H
