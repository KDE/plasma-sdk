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

#include "imageviewer.h"
#include "imageloader.h"

#include <QSvgWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QThreadPool>
#include <QTimer>

#include <KMessageBox>

ImageViewer::ImageViewer(QWidget* parent)
        : QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignHCenter);

    m_svgWidget = new QSvgWidget(this);
    layout->addWidget(m_svgWidget);

    m_label = new QLabel(this);
    layout->addWidget(m_label);

    m_reloadRequestTimer = new QTimer(this);
    connect(m_reloadRequestTimer, SIGNAL(timeout()), this, SLOT(reloadImage()));
}

void ImageViewer::loadImage(const QUrl& image)
{
    m_reloadRequestTimer->stop();

    m_image = image;

    if (image.path().endsWith(".svg") || image.path().endsWith(".svgz")) {
        loadSvg(image);
    } else {
        ImageLoader *loader = new ImageLoader(image, size(), this);
        connect(loader, SIGNAL(loadImage(QUrl,QSize,QImage)), this, SLOT(loadPixmap(QUrl,QSize,QImage)));
        QThreadPool::globalInstance()->start(loader);
    }
}

void ImageViewer::resizeEvent(QResizeEvent *)
{
    m_reloadRequestTimer->start();
}

void ImageViewer::reloadImage()
{
    loadImage(m_image);
}

void ImageViewer::loadPixmap(const QUrl &url, const QSize &s, const QImage& image)
{
    if (url == m_image && s == size()) {
        m_label->setPixmap(QPixmap::fromImage(image));
    }
}

void ImageViewer::loadSvg(const QUrl& image)
{
    m_svgWidget->load(image.path());
}

const QUrl ImageViewer::imagePath()
{
    return m_image;
}


