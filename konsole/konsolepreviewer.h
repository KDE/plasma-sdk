/*
 *   Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KONSOLEPREVIEWER_H
#define KONSOLEPREVIEWER_H

#include <QDockWidget>

class KTextEdit;

class KonsolePreviewer : public QDockWidget {

    Q_OBJECT

public:
    KonsolePreviewer(const QString & title, QWidget *parent = 0);

public Q_SLOTS:
    void clearOutput();
    void clearTmpFile();
    void populateKonsole();
    void saveOutput();

private:
    KTextEdit *m_textEdit;
    QString takeOutput() const;
};

#endif // KONSOLEPREVIEWER_H
