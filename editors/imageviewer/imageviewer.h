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

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>
#include <KUrl>
#include <QSize>
#include <QImage>

class ImageLoader;
class QLabel;
class QSvgWidget;

class ImageViewer : public QWidget
{
    Q_OBJECT

public:
    /**
     * A simple ctor.Nothing special.
     **/
    ImageViewer(const KUrl& image, QWidget *parent = 0);

    /**
     * Returns the directory for the image which will be displayed.
     * @see setImage()
     **/
    const KUrl imagePath();

    /**
     * Sets the directory for the image which will be displayed.
     **/
    void loadImage(const KUrl& image);

private Q_SLOTS:
    void loadPixmap(const QImage& image);

private:
    ImageLoader *m_loader;
    KUrl m_image;
    QSvgWidget *m_svgWidget;
    QLabel *m_label;

    void loadSvg(const KUrl& image);
};

#endif // IMAGEVIEWER_H
