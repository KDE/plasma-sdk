#include "mainwindow.h"
 
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
 
#include <QApplication>
 
MainWindow::MainWindow()
    : KParts::MainWindow( )
{
 
    // Setup our actions
    setupActions();
 
    // this routine will find and load our Part.  
    KLibFactory *factory = KLibLoader::self()->factory("katepart");
    if (factory)
    {
        // now that the Part is loaded, we cast it to a Part to get
        // our hands on it
        m_part = static_cast<KParts::ReadWritePart *>
                 (factory->create(this, "KatePart" ));
 
        if (m_part)
        {
            // tell the KParts::MainWindow that this is indeed
            // the main widget
            setCentralWidget(m_part->widget());
 
            setupGUI(ToolBar | Keys | StatusBar | Save);
 
            // and integrate the part's GUI with the shell's
            createGUI(m_part);
        }
    }
    else
    {
        // if we couldn't find our Part, we exit since the Shell by
        // itself can't do anything useful
        KMessageBox::error(this, "Could not find our Part!");
        qApp->quit();
        // we return here, cause qApp->quit() only means "exit the
        // next time we enter the event loop...
        return;
    }
}
 
MainWindow::~MainWindow()
{
}
 
void MainWindow::load(const KUrl& url)
{
    m_part->openUrl( url );
}
 
void MainWindow::setupActions()
{
    KStandardAction::open(this, SLOT(fileOpen()), 
        actionCollection());
    KStandardAction::quit(qApp, SLOT(closeAllWindows()),
        actionCollection());
}
 
void MainWindow::load()
{
    load(KFileDialog::getOpenUrl());
}