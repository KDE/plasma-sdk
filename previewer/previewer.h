/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef PREVIEWER_H
#define PREVIEWER_H

#include <QDockWidget>

// This is the base previewer dockwidget class
// All previewer types should inherit from this class
class Previewer : public QDockWidget {

    Q_OBJECT

signals:
    void refreshRequested();

public:
    Previewer(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
    Previewer(QWidget * parent = 0, Qt::WindowFlags flags = 0 );
    /**
     * Subclasses should override this method with actual previewing code
     */
    virtual void showPreview(const QString &packagePath) = 0;
    /**
     * Subclasses should override this method with code that refreshes the preview
     */
    virtual void refreshPreview() = 0;

public slots:
    /**
     * Emits refreshRequested(), which should signal the editor to save its contents
     * and then invoke refresh() on the previewer. All subclasses should call/connect to
     * this when the user requests a refresh.
     */
    void emitRefreshRequest();
    
};

#endif // PREVIEWER_H