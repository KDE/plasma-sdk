/*
  Copyright 2009 Riccardo Iaconelli <riccardo@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QWidget>
#include <QDomDocument>

namespace KTextEditor
{
    class Document;
    class View;
} // namespace KTextEditor

class PackageModel;

class TextEditor : public QWidget
{
public:
    TextEditor(KTextEditor::Document *editorPart, PackageModel *model, QWidget *parent = 0);
    void modifyToolBar(KTextEditor::View *view);
};

#endif // TEXTEDITOR_H

