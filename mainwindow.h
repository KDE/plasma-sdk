#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <kmainwindow.h>

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public KMainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget *parent = 0);
//         ~MainWindow();

    private:
        Ui::MainWindowClass *ui;
};

#endif // MAINWINDOW_H
