/*
 * This file is part of the vng project
 * Copyright (C) 2008 Thomas Zander <tzander@trolltech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LOGGER_P_H
#define LOGGER_P_H

#include "logger.h"
#include <QProcess>
#include <QThreadStorage>

class NullDevice : public QIODevice
{
public:
    virtual qint64 readData (char *data, qint64 maxlen) {
        Q_UNUSED(data);
        Q_UNUSED(maxlen);
        return 0;
    }
    virtual qint64 writeData (const char *data, qint64 len) {
        Q_UNUSED(data);
        return len;
    }
};

class LoggerPrivate : QObject
{
    Q_OBJECT
public:
    LoggerPrivate();
    ~LoggerPrivate();

    void insertPager();
    void removePager();
    bool startPager(const QString &program, const QStringList &arguments = QStringList());

    QTextStream & standardOut() {
        if (! standardOutStream.hasLocalData()) {
            QTextStream *out = new QTextStream(stdout);
            standardOutStream.setLocalData(out);
        }
        return *(standardOutStream.localData());
    }

    Logger::Verbosity verbosity;
    QTextStream dummy;
    QTextStream *pagerStream;
    QProcess *pager;
    bool colorPager;
    QThreadStorage<QTextStream*> standardOutStream;

private slots:
    void pagerFinished();
};

#endif
