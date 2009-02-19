#ifndef SVGVIEWER_H
#define SVGVIEWER_H

#include <QWidget>

namespace Plasma {
    class Svg;
};

class SvgViewer : public QWidget
{
    Q_OBJECT

public:
    SvgViewer( QWidget *parent );
    ~SvgViewer();

    void setFilename( const QString &filename );
    void readFile();

protected:
    void paintEvent( QPaintEvent *p );
//    void mouseReleaseEvent( QMouseEvent *e );
    void resizeEvent( QResizeEvent *e );

private:
    QString filename;
    Plasma::Svg *m_renderer;
};

#endif // SVGVIEWER_H
