/***************************************************************************
 *                                                                         *
 *   Copyright 2014-2015 Sebastian Kügler <sebas@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
 *                                                                         *
 ***************************************************************************/

#ifndef CUTTLEFISHPLUGIN_H
#define CUTTLEFISHPLUGIN_H

#include <KTextEditor/Plugin>
#include <KTextEditor/MainWindow>
#include <KTextEditor/Document>
#include <KTextEditor/View>

#include <QList>
#include <QMenu>
#include <QProcess>
#include <QVariant>


class CuttleFishPlugin : public KTextEditor::Plugin
{
    Q_OBJECT

public:
    explicit CuttleFishPlugin(QObject *parent = 0, const QList<QVariant> & = QList<QVariant>());
    virtual ~CuttleFishPlugin();

    QObject *createView(KTextEditor::MainWindow *mainWindow) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void contextMenuAboutToShow (KTextEditor::View *view, QMenu *menu);
    void documentCreated(KTextEditor::Document *document);

    void showCuttleFish();
    void iconPicked();

private:
    QAction m_action;
};


#endif // CUTTLEFISHPLUGIN_H
