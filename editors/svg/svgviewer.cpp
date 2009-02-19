#include <QPainter>

#include <plasma/svg.h>

#include "svgviewer.h"

/**
 * Q: Why not just use QSvgWidget?
 * A: We want to support plasma specific features such as panel svg, svgs
 * containing a stack of images etc.
 */

SvgViewer::SvgViewer( QWidget *parent )
    : QWidget(parent),
      m_renderer(0)
{

}

SvgViewer::~SvgViewer()
{

}

void SvgViewer::setFilename( const QString &filename )
{
    this->filename = filename;
}

void SvgViewer::readFile()
{
    m_renderer->deleteLater();
    m_renderer = new Plasma::Svg( this );
    m_renderer->setImagePath( filename );
    m_renderer->resize( size() );
    update();
}

void SvgViewer::paintEvent( QPaintEvent *e )
{
    QPainter p(this);
    m_renderer->paint( &p, 0, 0, width(), height() );
}

#if 0
void SvgViewer::mouseReleaseEvent( QMouseEvent *e )
{
}
#endif

void SvgViewer::resizeEvent( QResizeEvent *e )
{
    if ( !m_renderer )
	return;

    m_renderer->resize( size() );
}

