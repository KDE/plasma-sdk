/*
  Copyright (c) 2009 Richard Moore, <rich@kde.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef SVGVIEWER_H
#define SVGVIEWER_H

#include <QWidget>

namespace Plasma
{
class FrameSvg;
}

class SvgViewer : public QWidget
{
    Q_OBJECT

public:
    enum Mode {
        ModeSvg,
        ModeFrameSvg
    };

    SvgViewer(QWidget *parent);
    ~SvgViewer();

    void setFilename(const QString &filename);
    void readFile();

    void setMode(SvgViewer::Mode mode);
    SvgViewer::Mode mode() const;

protected:
    void paintEvent(QPaintEvent *p);
//    void mouseReleaseEvent( QMouseEvent *e );
    void resizeEvent(QResizeEvent *e);

    SvgViewer::Mode detectMode();

private:
    Mode m_mode;
    QString m_filename;
    Plasma::FrameSvg *m_renderer;
};

#endif // SVGVIEWER_H
