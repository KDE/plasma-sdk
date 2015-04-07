/*
 *  Copyright 2012 Giorgos Tsiapaliwkas <terietor@gmail.com>>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 */

#include "plasmakconfigxteditor.h"

PlasmaKConfigXtEditor::PlasmaKConfigXtEditor(QWidget* parent)
        : KConfigXtEditor(parent)
{
    connect(m_ui.srcRequester, SIGNAL(textChanged(const QString&)), this, SLOT(checkProjectPath(const QString&)));

    //we want only the xml files
    m_ui.srcRequester->setFilter("*.xml");

    //disable the widgets. The user hasn't give a path yet.
    enableWidgets(false);

    //we want the source relative ui to be visible.
    m_ui.srcLabel1->setVisible(true);
    m_ui.srcLabel2->setVisible(true);
    m_ui.srcRequester->setVisible(true);
    m_ui.srcSeparator->setVisible(true);
}

void PlasmaKConfigXtEditor::checkProjectPath(const QString& path)
{
    if (path.isEmpty()) {
        return;
    }

    //check if the files is an xml
    if(path.endsWith(".xml")) {
        enableWidgets(true);
        setFilename(QUrl(path));
        readFile();
    } else {
        enableWidgets(false);
    }
}

void PlasmaKConfigXtEditor::enableWidgets(bool enable)
{
    m_ui.twEntries->setEnabled(enable);
    m_ui.twGroups->setEnabled(enable);
    m_ui.pbAddGroup->setEnabled(enable);
    m_ui.pbDeleteGroup->setEnabled(enable);
    m_ui.pbAddEntry->setEnabled(enable);
    m_ui.pbDeleteEntry->setEnabled(enable);
}

