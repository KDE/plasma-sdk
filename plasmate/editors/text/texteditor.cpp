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
along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include "texteditor.h"
#include "../../packagemodel.h"

#include <QHBoxLayout>

#include <KService>
#include <KServiceTypeTrader>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/CommandInterface>
#include <KTextEditor/Document>
#include <ktexteditor/editor.h>
#include <KTextEditor/View>

#include <QAction>

TextEditor::TextEditor(KTextEditor::Document *editorPart, PackageModel *model, QWidget *parent)
        : QWidget(parent)
{
    QHBoxLayout *l = new QHBoxLayout(this);

    QWidget *centralWidget = editorPart->widget();
    KTextEditor::View *view = qobject_cast<KTextEditor::View*>(centralWidget);
    if (view) {
        view->setContextMenu(view->defaultContextMenu());

        //modify the toolbar
        modifyToolBar(view);

        KTextEditor::ConfigInterface *config = qobject_cast<KTextEditor::ConfigInterface*>(view);
        if (config) {
            config->setConfigValue("line-numbers", true);
            config->setConfigValue("dynamic-word-wrap", true);
        }

        config = dynamic_cast<KTextEditor::ConfigInterface*>(editorPart);
        if (config) {
            config->setConfigValue("backup-on-save-prefix", ".");
        }

        // set nice defaults for katepart
        KTextEditor::CommandInterface *command = dynamic_cast<KTextEditor::CommandInterface *>(editorPart->editor());
        QString ret;
        if (command) { //generic
            command->queryCommand("set-indent-mode")->exec(view, "set-indent-mode normal", ret); // more friendly
            command->queryCommand("set-replace-tabs")->exec(view, "set-replace-tabs 1", ret);// replaces tabs with spaces????
            command->queryCommand("set-indent-width")->exec(view, "set-indent-width 4", ret);//4 spaces, Plasma's general coding style
        }

        //we should be setting the specific editing indentation, highlighting based on the type of document
        if (model->implementationApi() == "declarativeappletscript" || model->implementationApi() == "javascript") {
            editorPart->setHighlightingMode("JavaScript");
        } else if (model->implementationApi() == "ruby-script") {
            editorPart->setHighlightingMode("ruby");
            if (command) {
                command->queryCommand("set-indent-width")->exec(view, "set-indent-width 2", ret);// 2 spaces recommended for ruby
            }
        }
        //there is no need for one more control. But we keep the code because it is easier to understand.
        //so keep the generic format.
        /*} else if (editorPart->setHighlightingMode() == "python") {
            continue;
            // Q: why we don't change the spaces?
            // A: 4 spaces are recommended for python
        }*/

    }

    l->addWidget(centralWidget);
}

void TextEditor::modifyToolBar(KTextEditor::View *view)
{
    //we don't want all the actions to be visible.
    //Q:why?
    //A:because Plasmate doesn't use actions like save and save as, automatically it saves
    //the projects

    //see this page, http://techbase.kde.org/Development/Architecture/KDE4/XMLGUI_Technology

    QStringList names;
    names << "file_save" << "file_save_as";

    QDomDocument doc = view->xmlguiBuildDocument();
    if  (doc.documentElement().isNull()) {
        doc = view->domDocument();
    }

    QDomElement e = doc.documentElement();
    removeNamedElementsRecursive(names, e);
    view->setXMLGUIBuildDocument(doc);

}

void TextEditor::removeNamedElementsRecursive(const QStringList &names, QDomNode &parent)
{
    QDomNode nchild;

    for (QDomNode child = parent.firstChild(); !child.isNull(); child = nchild) {
        removeNamedElementsRecursive(names, child);

        nchild = child.nextSibling(); // need to fetch next sibling here, as we might remove the child below
        if (child.isElement()) {
            QDomElement e = child.toElement();
            if (names.contains(e.attribute("name"))) {
                parent.removeChild(child);
            }
        }
    }
}

