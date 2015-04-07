/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef REMOTEINSTALLERCORE_H
#define REMOTEINSTALLERCORE_H

#include <QObject>
#include <QUrl>

class KJob;

class RemoteInstaller : public QObject
{
    Q_OBJECT
public:
    RemoteInstaller(const QString& username, const QString& hostname,
                    const QString& source, QWidget* parent = 0);

private Q_SLOTS:
    void doPlasmaPkg(KJob* job);

private:
    void doInstall();
    QUrl m_execUrl;
    QString m_plasmaPkgUrl;
    QString m_sourcePath;
    QUrl m_destinationPath;
    QWidget *m_widget;
};

#endif // PUBLISHER_H
