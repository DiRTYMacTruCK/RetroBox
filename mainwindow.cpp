#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>  // Required for QApplication::quit()
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect the Exit action to the function
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
}
MainWindow::~MainWindow()
{
    delete ui;
}

// Function to handle Exit action
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}
void MainWindow::openSettings()
{
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();  // This opens the settings dialog as a modal window
}
