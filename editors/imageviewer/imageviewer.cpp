/*
Copyright 2011 Giorgos Tsiapaliwkas <terietor@gmail.org>

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
#include <KMessageBox>

ImageViewer::ImageViewer(const KUrl& image,QWidget* parent)
        :QWidget(parent),
        m_loader(0)
{
    m_image = image;

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignHCenter);

    m_svgWidget = new QSvgWidget(this);
    layout->addWidget(m_svgWidget);

    m_label = new QLabel(this);
    layout->addWidget(m_label);

    setLayout(layout);

    loadImage(m_image);
}

void ImageViewer::loadImage(const KUrl& image)
{
    if(image.path().endsWith(".svg") || image.path().endsWith(".svgz")) {
        loadSvg(image);
    } else {
        m_loader = new ImageLoader(image, this);
        connect(m_loader, SIGNAL(loadImage(QImage)), this, SLOT(loadPixmap(QImage)));
        QThreadPool::globalInstance()->start(m_loader);
    }
}

void ImageViewer::loadPixmap(const QImage& image)
{
    setImageSize(image.size());
    m_label->setPixmap(QPixmap::fromImage(image));
}


void ImageViewer::loadSvg(const KUrl& image)
{
    m_svgWidget->load(image.path());
    setImageSize(m_svgWidget->size());
}

const KUrl ImageViewer::imagePath()
{
    return m_image;
}