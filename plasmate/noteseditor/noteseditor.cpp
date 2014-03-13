/*
Copyright 2014 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

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

#include "noteseditor.h"

#include <QFile>
#include <QDebug>

#include <KLocalizedString>
#include <KMimeTypeTrader>
#include <KParts/ReadWritePart>
#include <KService>
#include <KTextEditor/ConfigInterface>

NotesEditor::NotesEditor(const QString &projectPath, QWidget *parent)
    : QDockWidget(i18n("Project Notes"), parent),
      m_notesPart(0),
      m_projectPath(projectPath)
{
    setObjectName(QStringLiteral("Project Notes"));
    init();
}

NotesEditor::~NotesEditor()
{
    KParts::ReadWritePart *part = qobject_cast<KParts::ReadWritePart*>(m_notesPart);
    if (part) {
        part->save();
    }
}

void NotesEditor::init()
{
    KService::List offers = KMimeTypeTrader::self()->query("text/plain", "KParts/ReadWritePart");
    if (offers.isEmpty()) {
        offers = KMimeTypeTrader::self()->query("text/plain", "KParts/ReadOnlyPart");
    }

    if (!offers.isEmpty()) {
        QVariantList args;
        QString error;
        m_notesPart = dynamic_cast<KParts::ReadOnlyPart*>(
                          offers.at(0)->createInstance<KParts::Part>(
                              this, args, &error));

        if (!m_notesPart) {
            qDebug() << "Failed to load notes editor:" << error;
            return;
        }

        KTextEditor::ConfigInterface *config = dynamic_cast<KTextEditor::ConfigInterface*>(m_notesPart);
        if (config) {
            config->setConfigValue("backup-on-save-prefix", ".");
        }

        refreshNotes();
        setWidget(m_notesPart->widget());
    }

}

void NotesEditor::refreshNotes()
{
    if (!m_notesPart) {
        return;
    }

    KParts::ReadWritePart* part = qobject_cast<KParts::ReadWritePart*>(m_notesPart);

    // save notes if we previously had one open.
    if (part && part->isModified()) {
        part->save();
    }

    const QString notesFile = m_projectPath + QStringLiteral("/.NOTES");
    QFile notes(notesFile);

    if (!notes.exists()) {
        notes.open(QIODevice::WriteOnly);
        notes.close();
    }

    m_notesPart->openUrl(QUrl("file://" + notesFile));
}

