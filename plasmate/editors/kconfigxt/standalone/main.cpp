/*
 * Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <KLocale>

#include <QDialog>
#include <QVBoxLayout>
#include <QScopedPointer>

#include "plasmakconfigxteditor.h"
#include <qcommandlineparser.h>
#include <qcommandlineoption.h>

static const char version[] = "2.0";

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCommandLineParser parser;

    app.setApplicationVersion(version);
    parser.addVersionOption();
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    PlasmaKConfigXtEditor *editor = new PlasmaKConfigXtEditor();

    QScopedPointer<QDialog> dialog(new QDialog());
    QVBoxLayout layout;

    layout.addWidget(editor);
    dialog->setLayout(&layout);
    dialog->exec();

    return app.exec();
}


