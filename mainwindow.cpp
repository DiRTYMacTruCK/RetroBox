#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include "settingsdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , playlist(new QMediaPlaylist(this))
{
    ui->setupUi(this);
    mediaPlayer->setPlaylist(playlist);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(ui->actionOpen_Library, &QAction::triggered, this, &MainWindow::on_actionOpenLibrary_triggered);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked);
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

void MainWindow::on_actionOpenLibrary_triggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Open Library"), "",
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (!directoryPath.isEmpty()) {
        // Scan the directory for audio files
        QDir directory(directoryPath);
        QStringList filters;
        filters << "*.mp3" << "*.wav" << "*.flac" << "*.ogg";  // Add more audio file formats as needed
        QStringList audioFiles = directory.entryList(filters, QDir::Files | QDir::Readable, QDir::Name);

        // Clear any previous list and add new files
        ui->listWidget->clear();
        for (const QString &file : audioFiles) {
            ui->listWidget->addItem(file);
        }
    }
}
