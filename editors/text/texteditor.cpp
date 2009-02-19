/*
  Copyright (c) 2009 Riccardo Iaconelli <riccardo@kde.org>
 
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/
#include <kaction.h>
#include <kactioncollection.h>
#include <kconfig.h>
#include <kedittoolbar.h>
#include <kfiledialog.h>
#include <kshortcutsdialog.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kstandardaction.h>
#include <kstatusbar.h>
#include <kurl.h>
#include <kparts/part.h>

#include <QHBoxLayout>

#include "texteditor.h"
 
TextEditor::TextEditor(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout l;
    
    KLibFactory *factory = KLibLoader::self()->factory("katepart");
    if (factory) {
        // now that the Part is loaded, we cast it to a Part to get
        // our hands on it
        KParts::ReadWritePart *m_part = static_cast<KParts::ReadWritePart *>(factory->create(this, "KatePart" ));
 
        if (m_part) {
            // tell the KParts::MainWindow that this is indeed
            // the main widget
            l.addWidget(m_part->widget());
//             setCentralWidget(m_part->widget());
 
//             setupGUI(ToolBar | Keys | StatusBar | Save);
 
            // and integrate the part's GUI with the shell's
//             createGUI(m_part);
        } 
    } 
   
    setLayout(&l);
}
