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

#include <QHBoxLayout>

#include <KService>
#include <KServiceTypeTrader>
#include <KTextEditor/ConfigInterface>
#include <KTextEditor/Document>
#include <KTextEditor/View>

TextEditor::TextEditor(QWidget *parent)
        : QWidget(parent)
{
    QHBoxLayout *l = new QHBoxLayout(this);

    KService::List offers = KServiceTypeTrader::self()->query("KTextEditor/Document");
    foreach (const KService::Ptr service, offers) {
        kDebug() << "trying for ..." << service->name();
        KTextEditor::Document *editorPart = service->createInstance<KTextEditor::Document>(this);
        if (editorPart) {
            //FIXME: we should be setting the highlight based on the type of document
            //editorPart->setHighlightingMode("JavaScript");

            KTextEditor::View *view = editorPart->createView(this);
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

//             setupGUI(ToolBar | Keys | StatusBar | Save);

            // and integrate the part's GUI with the shell's
//             createGUI(m_part);
            l->addWidget(view);
        }
    }
}
