/*
 *   SPDX-FileCopyrightText: 2008 Aaron Seigo <aseigo@kde.org>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef SERVICEVIEWER_H
#define SERVICEVIEWER_H

#include <QDialog>
#include "ui_serviceviewer.h"

class KJob;
class QPushButton;

namespace Plasma
{
    class DataEngine;
    class Service;
} // namespace Plasma

class ServiceViewer : public QDialog, public Ui::ServiceViewer
{
    Q_OBJECT

public:
    ServiceViewer(Plasma::DataEngine *engine, const QString &m_source, QWidget *parent = nullptr);
    ~ServiceViewer() override;

private:
    void updateJobCount(int numberOfJobs);

private Q_SLOTS:
    void updateOperations();
    void startOperation();
    void operationSelected(const QString &operation);
    void operationResult(KJob *job);
    void engineDestroyed();

private:
    Plasma::DataEngine *m_engine;
    Plasma::Service *m_service;
    QString m_source;
    int m_operationCount;
    QPushButton *m_operationButton;
};

#endif

