#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include "settingsdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QDirIterator>
#include <QMediaMetaData>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mediaPlayer(new QMediaPlayer(this))
    , playlist(new QMediaPlaylist(this))
{
    ui->setupUi(this);
    mediaPlayer->setPlaylist(playlist);

    // Ensure mediaPlayer runs on the main thread
    mediaPlayer->moveToThread(QApplication::instance()->thread());

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(ui->actionOpenLibrary, &QAction::triggered, this, &MainWindow::on_actionOpenLibrary_triggered);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_songSelected);

    // Correct signal connection for Qt 6 (Updated method)
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged);
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
    // Open file dialog ONCE
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Open Library"), "",
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    if (directoryPath.isEmpty()) return; // Exit if no folder is selected

    ui->listWidget->clear();  // Clear previous list

    QDirIterator it(directoryPath, {"*.mp3", "*.wav", "*.flac", "*.ogg"},
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();

        QMediaPlayer tempPlayer;
        tempPlayer.setMedia(QUrl::fromLocalFile(filePath));

        // Wait for metadata to load
        QEventLoop loop;
        connect(&tempPlayer, &QMediaPlayer::mediaStatusChanged, &loop, &QEventLoop::quit);
        tempPlayer.play();
        loop.exec();

        QString title = tempPlayer.metaData(QMediaMetaData::Title).toString();
        QString artist = tempPlayer.metaData(QMediaMetaData::Author).toString();
        QString album = tempPlayer.metaData(QMediaMetaData::AlbumTitle).toString();

        if (title.isEmpty()) title = QFileInfo(filePath).fileName();  // Use filename if no metadata

        QString displayText = QString("%1 - %2 (%3)")
                                  .arg(artist.isEmpty() ? "Unknown Artist" : artist)
                                  .arg(title)
                                  .arg(album.isEmpty() ? "Unknown Album" : album);

        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, filePath);  // Store file path
        ui->listWidget->addItem(item);
    }
}

void MainWindow::on_playButton_clicked()
{
    if (mediaPlayer->state() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
    } else {
        mediaPlayer->play();
    }
}

void MainWindow::on_songSelected(QListWidgetItem *item)
{
    QString filePath = item->data(Qt::UserRole).toString();

    // Set media file using setMedia()
    mediaPlayer->setMedia(QUrl::fromLocalFile(filePath));

    // Manually update metadata
    on_metaDataChanged();

    // Start playing
    mediaPlayer->play();
}

void MainWindow::fetchMetadata(const QString &filePath, QString &title, QString &artist, QString &album)
{
    QMediaPlayer tempPlayer;
    tempPlayer.setMedia(QUrl::fromLocalFile(filePath));

    // Wait for metadata to load
    QEventLoop loop;
    connect(&tempPlayer, &QMediaPlayer::mediaStatusChanged, &loop, &QEventLoop::quit);
    tempPlayer.play();
    loop.exec();

    title = tempPlayer.metaData(QMediaMetaData::Title).toString();
    artist = tempPlayer.metaData(QMediaMetaData::Author).toString();
    album = tempPlayer.metaData(QMediaMetaData::AlbumTitle).toString();

    if (title.isEmpty()) title = QFileInfo(filePath).fileName();  // Use filename if no metadata
}

void MainWindow::on_metaDataChanged()
{
    if (mediaPlayer->isMetaDataAvailable()) {
        QVariant titleVariant = mediaPlayer->metaData(QMediaMetaData::Title);
        QString title = titleVariant.isValid() ? titleVariant.toString() : "Unknown Title";

        QVariant artistVariant = mediaPlayer->metaData(QMediaMetaData::Author);
        QString artist = artistVariant.isValid() ? artistVariant.toString() : "Unknown Artist";

        ui->labelTitle->setText(title + " - " + artist);
    }
}

void MainWindow::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch (status) {
    case QMediaPlayer::NoMedia:
        qDebug() << "No media loaded.";
        break;
    case QMediaPlayer::LoadingMedia:
        qDebug() << "Loading media...";
        break;
    case QMediaPlayer::LoadedMedia:
        qDebug() << "Media loaded.";
        break;
    case QMediaPlayer::BufferingMedia:
        qDebug() << "Buffering...";
        break;
    case QMediaPlayer::StalledMedia:
        qDebug() << "Playback stalled.";
        break;
    case QMediaPlayer::EndOfMedia:
        qDebug() << "End of media reached.";
        break;
    case QMediaPlayer::InvalidMedia:
        qDebug() << "Invalid media.";
        break;
    default:
        break;
    }
}
