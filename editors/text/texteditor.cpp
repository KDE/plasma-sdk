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

    KTextEditor::View *view = editorPart->createView(this);
    if (view) {
        view->setContextMenu(view->defaultContextMenu());

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

    l->addWidget(view);
}

void TextEditor::modifyToolBar(KToolBar* toolbar)
{
    //we don't want all the actions to be visible.
    //Q:why?
    //A:because Plasmate doesn't use actions like save and save as, automatically it saves
    //the projects
    foreach(QAction *action, toolbar->actions()) {
        if (action->text() == "&Save" || action->text() == "Save &As...") {
            action->setVisible(false);
        }
    }
}

