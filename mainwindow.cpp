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
    , mediaPlayer(new QMediaPlayer(this))
    , currentTrackIndex(-1)  // Initialize with invalid index
{
    ui->setupUi(this);

    // Create "Now Playing" labels
    nowPlayingLabel = new QLabel("Now Playing:", this);
    titleLabel = new QLabel("Title: Unknown", this);
    artistLabel = new QLabel("Artist: Unknown", this);
    albumLabel = new QLabel("Album: Unknown", this);
    yearLabel = new QLabel("Year: Unknown", this);

    // Position the labels in your UI (adjust as needed)
    nowPlayingLabel->setGeometry(20, 250, 200, 30);
    titleLabel->setGeometry(20, 280, 400, 30);
    artistLabel->setGeometry(20, 310, 400, 30);
    albumLabel->setGeometry(20, 340, 400, 30);
    yearLabel->setGeometry(20, 370, 400, 30);

    // Connect metadata signal
    connect(mediaPlayer, &QMediaPlayer::metaDataChanged, this, &MainWindow::on_metaDataChanged);
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
    } else if (currentTrackIndex >= 0 && currentTrackIndex < trackList.size()) {
        mediaPlayer->play();
    } else if (!trackList.isEmpty()) {
        currentTrackIndex = 0; // Play the first song if none is selected
        mediaPlayer->setSource(QUrl::fromLocalFile(trackList[currentTrackIndex]));
        mediaPlayer->play();
    }
}

void MainWindow::on_songSelected(QListWidgetItem *item)
{
    QString filePath = item->data(Qt::UserRole).toString();
    currentTrackIndex = trackList.indexOf(filePath);

    if (currentTrackIndex != -1) {
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
        qDebug() << "Loaded song: " << filePath;
        mediaPlayer->play();
    }
}

void MainWindow::on_metaDataChanged()
{
    if (mediaPlayer->metaData().isEmpty()) {
        qDebug() << "Metadata not available!";
        return;
    }

    QString title = mediaPlayer->metaData().value(QMediaMetaData::Title).toString();
    QString artist = mediaPlayer->metaData().value(QMediaMetaData::ContributingArtist).toString();
    QString album = mediaPlayer->metaData().value(QMediaMetaData::AlbumTitle).toString();

    // Extract the year from the Date metadata
    QString year;
    if (!mediaPlayer->metaData().value(QMediaMetaData::Date).isNull()) {
        year = mediaPlayer->metaData().value(QMediaMetaData::Date).toDate().toString("yyyy");
    }

    if (title.isEmpty()) title = "Unknown Title";
    if (artist.isEmpty()) artist = "Unknown Artist";
    if (album.isEmpty()) album = "Unknown Album";
    if (year.isEmpty()) year = "Unknown Year";

    titleLabel->setText("Title: " + title);
    artistLabel->setText("Artist: " + artist);
    albumLabel->setText("Album: " + album);
    yearLabel->setText("Year: " + year);

    qDebug() << "Now Playing - Title: " << title << " | Artist: " << artist << " | Album: " << album << " | Year: " << year;
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
        playNext(); // Play next track when current one ends
        break;
    case QMediaPlayer::InvalidMedia:
        qDebug() << "Invalid media.";
        break;
    default:
        break;
    }
}

void MainWindow::playNext()
{
    if (trackList.isEmpty() || currentTrackIndex == -1) return;

    currentTrackIndex++;
    if (currentTrackIndex >= trackList.size()) {
        currentTrackIndex = 0; // Loop back to the first track
    }

    mediaPlayer->setSource(QUrl::fromLocalFile(trackList[currentTrackIndex]));
    mediaPlayer->play();
}
