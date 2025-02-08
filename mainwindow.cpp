#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QMediaPlayer>
#include <QDirIterator>
#include <QMediaMetaData>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mediaPlayer(new QMediaPlayer(this))  // No QMediaPlaylist
    , currentTrackIndex(-1)
{
    ui->setupUi(this);

    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(ui->actionOpenLibrary, &QAction::triggered, this, &MainWindow::on_actionOpenLibrary_triggered);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_songSelected);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::metaDataChanged, this, &MainWindow::on_metaDataChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_actionOpenLibrary_triggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Open Library"), "",
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directoryPath.isEmpty()) return;

    ui->listWidget->clear();
    trackList.clear();

    QDirIterator it(directoryPath, {"*.mp3", "*.wav", "*.flac", "*.ogg"},
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        QString displayText = QFileInfo(filePath).fileName();

        QListWidgetItem *item = new QListWidgetItem(displayText);
        item->setData(Qt::UserRole, filePath);
        ui->listWidget->addItem(item);

        trackList.append(filePath);  // Store file paths manually
    }
}

void MainWindow::on_playButton_clicked()
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
    } else {
        mediaPlayer->play();
    }
}

void MainWindow::on_songSelected(QListWidgetItem *item)
{
    QString filePath = item->data(Qt::UserRole).toString();
    currentTrackIndex = trackList.indexOf(filePath);

    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    qDebug() << "Loaded song: " << filePath;
    mediaPlayer->play();
}

void MainWindow::on_metaDataChanged()
{
    if (mediaPlayer->metaData().isEmpty()) {  // ✅ Check if metadata exists in Qt 6
        qDebug() << "Metadata not available!";
        return;
    }

    QString title = mediaPlayer->metaData().value(QMediaMetaData::Title).toString();
    QString artist = mediaPlayer->metaData().value(QMediaMetaData::ContributingArtist).toString();

    if (title.isEmpty()) title = "Unknown Title";
    if (artist.isEmpty()) artist = "Unknown Artist";

    ui->labelTitle->setText(title + " - " + artist);
    qDebug() << "Updated Metadata - Title: " << title << " | Artist: " << artist;
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
        on_metaDataChanged();
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
