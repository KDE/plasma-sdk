#ifndef SVGVIEWER_H
#define SVGVIEWER_H

#include <QWidget>

namespace Plasma {
    class FrameSvg;
};

class SvgViewer : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
	ModeSvg,
	ModeFrameSvg
    };

    SvgViewer( QWidget *parent );
    ~SvgViewer();

    void setFilename( const QString &filename );
    void readFile();

    void setMode( SvgViewer::Mode mode );
    SvgViewer::Mode mode() const;

protected:
    void paintEvent( QPaintEvent *p );
//    void mouseReleaseEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );

    SvgViewer::Mode detectMode();

private:
    Mode m_mode;
    QString m_filename;
    Plasma::FrameSvg *m_renderer;
};

#endif // SVGVIEWER_H
