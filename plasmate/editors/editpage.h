/*

Copyright 2009-2010 Laurent Montel <montel@kde.org>
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

#ifndef EDITPAGE_H
#define EDITPAGE_H

#include <QDockWidget>
#include <QModelIndex>
#include <QTreeView>
#include <KParts/ReadOnlyPart>

namespace KTextEditor {
    class View;
};

class ImageViewerPart;
class KConfigXtEditorPart;
class MetaDataEditor;
class PackageModel;
class PackageHandler;

class EditPage : public QTreeView
{
    Q_OBJECT

public:
    explicit EditPage(PackageHandler *packageHandler, QWidget *parent = 0);

Q_SIGNALS:
    void loadRequiredEditor(KParts::ReadOnlyPart *editor);

private Q_SLOTS:
    void findEditor(const QModelIndex &index);
    void showTreeContextMenu(const QPoint&);
    void doDelete(bool);

private:
    QMenu *m_contextMenu;
    QUrl m_path;
    bool hasExtension(const QString &filename);
    void imageDialog(const QString &filter, const QString& destinationPath);

    QWeakPointer<ImageViewerPart> m_imageViewer;
    QWeakPointer<KConfigXtEditorPart> m_kconfigXtEditor;
    QWeakPointer<MetaDataEditor> m_metaEditor;
    QWeakPointer<KTextEditor::View> m_textEditor;

    PackageModel *m_packageModel;
    PackageHandler *m_packageHandler;
};




class FileList : public QDockWidget
{
    Q_OBJECT

public:

    FileList(PackageHandler *packageHandlder, QWidget *parent = 0);

    EditPage *editPage() const;

private:
    EditPage *m_editPage;
};


#endif

