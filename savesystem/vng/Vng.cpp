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
#include "Vng.h"

#include <QProcess>
#include <QDebug>

using namespace Vng;

qint64 Vng::readLine(QIODevice *device, char *buffer, int maxLength)
{
    device->waitForReadyRead(-1);
    return device->readLine(buffer, maxLength);
}

bool Vng::copyFile(QIODevice &from, QIODevice &to)
{
    to.open(QIODevice::WriteOnly);
    char buf[4096];
    while (true) {
        from.waitForReadyRead(-1);
        qint64 len = from.read(buf, sizeof(buf));
        if (len <= 0) { // done!
            to.close();
            break;
        }
        while(len > 0) {
            qint64 written = to.write(buf, len);
            if (written == -1)    // write error!
                return false;
            len -= written;
        }
    }
    return true;
}
