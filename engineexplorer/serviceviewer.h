/*
 *   Copyright 2008 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 2,
 *   or (at your option) any later version.
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

#ifndef SERVICEVIEWER_H
#define SERVICEVIEWER_H

#include <KDialog>
#include "ui_serviceviewer.h"

namespace Plasma
{
    class DataEngine;
    class Service;
    class ServiceJob;
} // namespace Plasma

class ServiceViewer : public KDialog, public Ui::ServiceViewer
{
    Q_OBJECT

public:
    ServiceViewer(Plasma::DataEngine *engine, const QString &m_source, QWidget *parent = 0);
    ~ServiceViewer();

private:
    void updateJobCount(int numberOfJobs);

private slots:
    void updateOperations();
    void startOperation();
    void operationSelected(const QString &operation);
    void operationResult(Plasma::ServiceJob *job);
    void engineDestroyed();

private:
    Plasma::DataEngine *m_engine;
    Plasma::Service *m_service;
    QString m_source;
    int m_operationCount;
};

#endif

