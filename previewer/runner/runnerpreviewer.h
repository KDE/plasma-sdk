/*
 * Copyright 2010 Lim Yuen Hoe <yuenhoe@hotmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifndef RUNNERPREVIEWER_H
#define RUNNERPREVIEWER_H

#include "../previewer.h"

class KLineEdit;
class QListWidget;

namespace Plasma {
  class AbstractRunner;
}

class RunnerPreviewer : public Previewer {

    Q_OBJECT

public:
    RunnerPreviewer(const QString & title, QWidget * parent = 0, Qt::WindowFlags flags = 0 );
    void showPreview(const QString &packagePath);
    void refreshPreview();

public slots:
    void doQuery();

private:
    KLineEdit* m_edit;
    QListWidget* m_matches;
    Plasma::AbstractRunner* m_runner;
};

#endif // RUNNERPREVIEWER_H