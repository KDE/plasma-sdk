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

#include "logger.h"
#include "logger_p.h"
#include <QDebug>
#include <QThread>

LoggerPrivate::LoggerPrivate()
    : verbosity(Logger::Chatty),
    dummy(new NullDevice()),
    pagerStream(0),
    pager(0),
    colorPager(false)
{
}

LoggerPrivate::~LoggerPrivate()
{
    removePager();
}

void LoggerPrivate::insertPager()
{
    if (pager)
        return; // already present.
    pager = new QProcess();
    connect(pager, SIGNAL(finished(int )), this, SLOT(pagerFinished()));
    pager->setProcessChannelMode(QProcess::ForwardedChannels);
    colorPager = true;
    if (! startPager("less", QStringList() << "-R" << "-E")) {
        colorPager = false;
        if (! startPager("less", QStringList() << "-E")) {
            if (! startPager("more")) {
                delete pager;
                pager = 0;
                return;
            }
        }
    }
    pagerStream = new QTextStream(pager);
}

void LoggerPrivate::removePager()
{
    if (pager == 0)
        return;
    pagerStream->flush();
    pager->closeWriteChannel();
    pager->waitForFinished(-1);
    delete pager;
    pager = 0;
    delete pagerStream;
    pagerStream = 0;
}

bool LoggerPrivate::startPager(const QString &program, const QStringList &arguments)
{
    Q_ASSERT(pager);
    pager->start(program, arguments, QIODevice::ReadWrite);
    if (! pager->waitForStarted()) {
        if (pager->state() != QProcess::NotRunning) { // time out
            standardOut() << "ERROR: the pager failed to start" << endl;
            pager->kill();
        }
        return false;
    }
    if (pager->state() != QProcess::Running) // it already 'finished'
        return false;
    return true;
}

void LoggerPrivate::pagerFinished()
{
    if (! pager)
        return;
    pager->close();
}

Q_GLOBAL_STATIC(LoggerPrivate, _vng_logger_private)


Logger::Logger()
    : d(_vng_logger_private())
{
}

void Logger::setVerbosity(Verbosity verbosity)
{
    _vng_logger_private()->verbosity = verbosity;
}

Logger::Verbosity Logger::verbosity()
{
    return _vng_logger_private()->verbosity;
}

QTextStream & Logger::standardOut()
{
    LoggerPrivate *self = _vng_logger_private();
    if (self->pagerStream)
        return *self->pagerStream;
    return self->standardOut();
}

QTextStream &Logger::log(Verbosity verbosity)
{
    LoggerPrivate *self = _vng_logger_private();
    if (self->verbosity >= verbosity) {
        if (self->pagerStream && QThread::currentThread() == self->pager->thread()) {
            self->pager->waitForBytesWritten();
            return *self->pagerStream;
        }
        return self->standardOut();
    }
    return self->dummy;
}

QTextStream & Logger::debug()
{
    return log(Debug);
}

QTextStream & Logger::info()
{
    return log(Verbose);
}

QTextStream & Logger::warn()
{
    return log(Chatty);
}

QTextStream & Logger::error()
{
    return log(Quiet);
}

void Logger::startPager()
{
    _vng_logger_private()->insertPager();
}

void Logger::stopPager()
{
    _vng_logger_private()->removePager();
}

void Logger::flushPager()
{
    QProcess *pager = _vng_logger_private()->pager;
    if (pager)
        pager->waitForBytesWritten();
}

bool Logger::hasNonColorPager()
{
    if (_vng_logger_private()->pager == 0)
        return false;
    return ! _vng_logger_private()->colorPager;
}
