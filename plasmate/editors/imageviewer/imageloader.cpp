/*
Copyright 2011 Giorgos Tsiapaliokas <giorgos.tsiapaliokas@kde.org>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "imageloader.h"

ImageLoader::ImageLoader(const QUrl& image, const QSize &size, QObject *parent)
     : QObject(parent),
       m_image(image),
       m_size(size)
{
    setAutoDelete(true);
}

void ImageLoader::run()
{
    QImage image(m_image.path(), "PNG JPG GIF JPEG SVG SVGZ");
    QSize newSize = image.size();

    if (newSize.width() > m_size.width()) {
        newSize.setWidth(m_size.width());
    }

    if (newSize.height() > m_size.height()) {
        newSize.setHeight(m_size.height());
    }

    if (newSize != image.size()) {
        image = image.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    emit loadImage(m_image, m_size, image);
}

#include "imageloader.moc"

