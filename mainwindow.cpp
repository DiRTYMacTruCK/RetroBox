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
#include <QAudioOutput>
#include <QSlider>
#include <QLabel>

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

    // Create playback progress bar
    progressBar = new QSlider(Qt::Horizontal, this);
    progressBar->setRange(0, 100);  // Progress in percent

    // Create volume slider
    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);  // Default to max volume

    // Position UI elements
    nowPlayingLabel->setGeometry(20, 250, 200, 30);
    titleLabel->setGeometry(20, 280, 400, 30);
    artistLabel->setGeometry(20, 310, 400, 30);
    albumLabel->setGeometry(20, 340, 400, 30);
    yearLabel->setGeometry(20, 370, 400, 30);
    progressBar->setGeometry(20, 400, 300, 20);
    volumeSlider->setGeometry(20, 430, 200, 20);

    // Ensure an audio output is set (fixes no sound issue)
    QAudioOutput *audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0);

    // Connect volume control
    connect(volumeSlider, &QSlider::valueChanged, this, [audioOutput](int value) {
        audioOutput->setVolume(value / 100.0);
    });

    // Connect metadata and playback signals
    connect(mediaPlayer, &QMediaPlayer::metaDataChanged, this, &MainWindow::on_metaDataChanged);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);

    qDebug() << "🔊 Audio output initialized. Player ready!";
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
    if (!item) {
        qDebug() << "Error: Selected item is null.";
        return;
    }

    QString filePath = item->data(Qt::UserRole).toString();
    qDebug() << "Selected file path: " << filePath;

    if (filePath.isEmpty()) {
        qDebug() << "Error: Empty file path!";
        return;
    }

    mediaPlayer->stop();  // Stop any existing playback

    qDebug() << "Setting new source: " << filePath;
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));

    // Debug: Confirm file path is in trackList
    int index = trackList.indexOf(filePath);
    if (index == -1) {
        qDebug() << "Error: File not found in trackList!";
    } else {
        qDebug() << "File is in trackList at index: " << index;
        currentTrackIndex = index;
    }

    // Wait for media to be loaded before playing
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {
            qDebug() << "✅ Media Loaded! Playing...";
            mediaPlayer->play();
        } else if (status == QMediaPlayer::InvalidMedia) {
            qDebug() << "❌ Error: Invalid media file!";
        }
    });

    // Force metadata update
    on_metaDataChanged();
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

void MainWindow::on_positionChanged(qint64 position)
{
    if (mediaPlayer->duration() > 0) {
        progressBar->setValue(static_cast<int>((position * 100) / mediaPlayer->duration()));
    }
}

void MainWindow::on_durationChanged(qint64 duration)
{
    progressBar->setRange(0, static_cast<int>(duration));
}
