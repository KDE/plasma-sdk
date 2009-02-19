#include <QPainter>
#include <KDebug>

#include <plasma/framesvg.h>

#include "svgviewer.h"

/**
 * Q: Why not just use QSvgWidget?
 * A: We want to support plasma specific features such as panel svg, svgs
 * containing a stack of images etc.
 */

SvgViewer::SvgViewer( QWidget *parent )
    : QWidget(parent),
      m_mode(SvgViewer::ModeSvg),
      m_renderer(0)
{

}

SvgViewer::~SvgViewer()
{

}

void SvgViewer::setFilename( const QString &filename )
{
    m_filename = filename;
}

void SvgViewer::readFile()
{
    m_renderer->deleteLater();
    m_renderer = new Plasma::FrameSvg( this );
    m_renderer->setImagePath( m_filename );
    if ( !m_renderer->isValid() ) {
	kWarning() << "The svg file " << m_filename << "was invalid";
    }
    m_mode = detectMode();
    m_renderer->resize( size() );
}

SvgViewer::Mode SvgViewer::detectMode()
{
    if ( !m_renderer ) {
	kWarning() << "We can't detect the mode when there's no renderer";
	return SvgViewer::ModeSvg;
    }

    if ( m_renderer->hasElement(QString("topleft"))
	 && m_renderer->hasElement(QString("top"))
	 && m_renderer->hasElement(QString("bottom"))
	 && m_renderer->hasElement(QString("bottomright")) )
	return SvgViewer::ModeFrameSvg;

    return SvgViewer::ModeSvg;
}

void SvgViewer::setMode( SvgViewer::Mode mode )
{
    m_mode = mode;
    update();
}

SvgViewer::Mode SvgViewer::mode() const
{
    return m_mode;
}

void SvgViewer::paintEvent( QPaintEvent *e )
{
    QPainter p(this);

    if ( m_mode == SvgViewer::ModeSvg )
	m_renderer->paint( &p, 0, 0, width(), height() );
    else if ( m_mode == SvgViewer::ModeFrameSvg )
	m_renderer->paintFrame( &p );
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

    if ( m_mode == SvgViewer::ModeSvg )
	m_renderer->resize( size() );
    else if ( m_mode == SvgViewer::ModeFrameSvg )
	m_renderer->resizeFrame( size() );
    update();
}

