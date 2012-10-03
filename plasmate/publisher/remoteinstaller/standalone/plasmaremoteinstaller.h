/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#ifndef PLASMAREMOTEINSTALLER_H
#define PLASMAREMOTEINSTALLER_H

#include "../remoteinstallerdialog.h"

class PlasmaRemoteInstaller : public RemoteInstallerDialog
{
    Q_OBJECT;
public:
    PlasmaRemoteInstaller(QWidget* parent = 0);

public Q_SLOTS:
    void checkProjectPath(const QString& path);
private:
};

#endif // PUBLISHER_H
