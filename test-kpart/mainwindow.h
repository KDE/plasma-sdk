#ifndef KPARTTUTORIAL1_H        
#define KPARTTUTORIAL1_H        
                                
#include <kparts/mainwindow.h>  
                                
/**                             
 * This is the application "Shell".  It has a menubar, toolbar, and
 * statusbar but relies on the "Part" to do all the real work.     
 *                                                                 
 * @short Application Shell                                        
 * @author Developer <developer@kde.org>                           
 * @version 0.1
 */
class MainWindow : public KParts::MainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    MainWindow();
 
    /**
     * Default Destructor
     */
    virtual ~MainWindow();
 
    /**
     * Use this method to load whatever file/URL you have
     */
    void load(const KUrl& url);
 
    /**
     * Use this method to display an openUrl dialog and
     * load the URL that gets entered
     */
    void load();
 
private:
    void setupActions();
 
private:
    KParts::ReadWritePart *m_part;
};
 
#endif // KPARTTUT1_H