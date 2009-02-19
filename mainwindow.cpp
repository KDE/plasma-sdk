#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : KMainWindow(parent), ui(new Ui::MainWindowClass)
{
    ui->setupUi(this);
}

// MainWindow::~MainWindow()
// {
//     delete ui;
// }
