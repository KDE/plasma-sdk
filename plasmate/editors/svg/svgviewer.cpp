/*
  Copyright 2009 Richard Moore, <rich@kde.org>

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

#include <QPainter>
#include <QDebug>

#include <plasma/framesvg.h>

#include "svgviewer.h"

/**
 * Q: Why not just use QSvgWidget?
 * A: We want to support plasma specific features such as panel svg, svgs
 * containing a stack of images etc.
 */

SvgViewer::SvgViewer(QWidget *parent)
        : QWidget(parent),
        m_mode(SvgViewer::ModeSvg),
        m_renderer(0)
{

}

SvgViewer::~SvgViewer()
{

}

void SvgViewer::setFilename(const QString &filename)
{
    m_filename = filename;
}

void SvgViewer::readFile()
{
    m_renderer->deleteLater();
    m_renderer = new Plasma::FrameSvg(this);
    m_renderer->setImagePath(m_filename);
    if (!m_renderer->isValid()) {
        qWarning() << "The svg file " << m_filename << "was invalid";
    }
    m_mode = detectMode();
    m_renderer->resize(size());
}

SvgViewer::Mode SvgViewer::detectMode()
{
    if (!m_renderer) {
        qWarning() << "We can't detect the mode when there's no renderer";
        return SvgViewer::ModeSvg;
    }

    if (m_renderer->hasElement(QString("topleft"))
            && m_renderer->hasElement(QString("top"))
            && m_renderer->hasElement(QString("bottom"))
            && m_renderer->hasElement(QString("bottomright")))
        return SvgViewer::ModeFrameSvg;

    return SvgViewer::ModeSvg;
}

void SvgViewer::setMode(SvgViewer::Mode mode)
{
    m_mode = mode;
    update();
}

SvgViewer::Mode SvgViewer::mode() const
{
    return m_mode;
}

void SvgViewer::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    if (m_mode == SvgViewer::ModeSvg)
        m_renderer->paint(&p, 0, 0, width(), height());
    else if (m_mode == SvgViewer::ModeFrameSvg)
        m_renderer->paintFrame(&p);
}

#if 0
void SvgViewer::mouseReleaseEvent(QMouseEvent *e)
{
}
#endif

void SvgViewer::resizeEvent(QResizeEvent *)
{
    if (!m_renderer)
        return;

    if (m_mode == SvgViewer::ModeSvg)
        m_renderer->resize(size());
    else if (m_mode == SvgViewer::ModeFrameSvg)
        m_renderer->resizeFrame(size());
    update();
}

