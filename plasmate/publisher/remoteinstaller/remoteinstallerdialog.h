/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef REMOTEINSTALLERDIALOG_H
#define REMOTEINSTALLERDIALOG_H

#include <QDialog>

#include "ui_remoteinstaller.h"

class RemoteInstaller;

class RemoteInstallerDialog : public QDialog
{
    Q_OBJECT;
public:
    RemoteInstallerDialog(QWidget* parent = 0);

    QString packagePath() const;
    void setPackagePath(const QString& path);

private Q_SLOTS:
    void install();
    void checkInformations();

protected:
    Ui::RemoteInstaller m_ui;

private:
    RemoteInstaller *m_installer;
    QString m_packagePath;
    QString m_username;
    QString m_hostname;
};

#endif // PUBLISHER_H
