#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include "settingsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}
void MainWindow::openSettings()
{
    SettingsDialog settingsDialog(this);
    settingsDialog.exec();
}
