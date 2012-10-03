/*
Copyright 2011 Giorgos Tsiapaliwkas <terietor@gmail.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IMAGELOADER_H
#define IMAGELOADER_H

#include <KUrl>
#include <QRunnable>
#include <QObject>
#include <QImage>

class ImageLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
    /**
     * You just have to create an instance of the object.
     * Don't do anything else.
     **/
    ImageLoader(const KUrl& image, const QSize &size, QObject *parent = 0);

    void run();

Q_SIGNALS:
    void loadImage(const KUrl &url, const QSize &size, const QImage& image);

private:
    KUrl m_image;
    QSize m_size;
};

#endif // IMAGELOADER_H
