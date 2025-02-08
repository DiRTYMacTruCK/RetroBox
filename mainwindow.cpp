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
    , currentTrackIndex(-1)
{
    ui->setupUi(this);

    nowPlayingLabel = new QLabel("Now Playing:", this);
    titleLabel = new QLabel("Title: Unknown", this);
    artistLabel = new QLabel("Artist: Unknown", this);
    albumLabel = new QLabel("Album: Unknown", this);
    yearLabel = new QLabel("Year: Unknown", this);

    progressBar = new QSlider(Qt::Horizontal, this);
    progressBar->setRange(0, 100);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(100);

    nowPlayingLabel->setGeometry(20, 250, 200, 30);
    titleLabel->setGeometry(20, 280, 400, 30);
    artistLabel->setGeometry(20, 310, 400, 30);
    albumLabel->setGeometry(20, 340, 400, 30);
    yearLabel->setGeometry(20, 370, 400, 30);
    progressBar->setGeometry(20, 400, 300, 20);
    volumeSlider->setGeometry(20, 430, 200, 20);

    QAudioOutput *audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    audioOutput->setVolume(1.0);

    connect(volumeSlider, &QSlider::valueChanged, this, [audioOutput](int value) {
        audioOutput->setVolume(value / 100.0);
    });
    connect(mediaPlayer, &QMediaPlayer::metaDataChanged, this, &MainWindow::on_metaDataChanged);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
    connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::playSelectedSong);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &MainWindow::on_songSelected); // ✅ Ensure this is connected
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::on_stopButton_clicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::on_nextButton_clicked);
    connect(ui->prevButton, &QPushButton::clicked, this, &MainWindow::on_prevButton_clicked);

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

        trackList.append(filePath);
    }
}

void MainWindow::on_playButton_clicked()
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
        qDebug() << "⏸ Playback paused.";
    } else {
        mediaPlayer->play();
        qDebug() << "▶ Playback started.";
    }
}

void MainWindow::on_stopButton_clicked()
{
    mediaPlayer->stop();
    qDebug() << "⏹ Playback stopped.";
}

void MainWindow::on_nextButton_clicked()
{
    playNext();  // Reuses existing playNext() function
    qDebug() << "⏭ Playing next track.";
}

void MainWindow::on_prevButton_clicked()
{
    if (trackList.isEmpty() || currentTrackIndex == -1) return;

    currentTrackIndex--;  // Move to the previous track

    if (currentTrackIndex < 0) {
        currentTrackIndex = trackList.size() - 1;  // Loop back to last track
    }

    QString prevTrackPath = trackList[currentTrackIndex];

    if (!prevTrackPath.isEmpty()) {
        qDebug() << "⏮ Playing previous track: " << prevTrackPath;
        mediaPlayer->setSource(QUrl::fromLocalFile(prevTrackPath));
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

    mediaPlayer->stop();
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();

    int index = trackList.indexOf(filePath);
    if (index != -1) {
        currentTrackIndex = index;
    }

    on_metaDataChanged();
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

void MainWindow::on_metaDataChanged()
{
    if (mediaPlayer->metaData().isEmpty()) {
        qDebug() << "Metadata not available!";
        return;
    }

    QString title = mediaPlayer->metaData().value(QMediaMetaData::Title).toString();
    QString artist = mediaPlayer->metaData().value(QMediaMetaData::ContributingArtist).toString();
    QString album = mediaPlayer->metaData().value(QMediaMetaData::AlbumTitle).toString();
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
        playNext();
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
    if (trackList.isEmpty() || currentTrackIndex == -1) {
        qDebug() << "No tracks in the playlist or invalid index.";
        return;
    }

    currentTrackIndex++;

    if (currentTrackIndex >= trackList.size()) {
        currentTrackIndex = 0;
    }

    QString nextTrackPath = trackList[currentTrackIndex];

    if (!nextTrackPath.isEmpty()) {
        qDebug() << "Now playing next track: " << nextTrackPath;
        mediaPlayer->setSource(QUrl::fromLocalFile(nextTrackPath));
        mediaPlayer->play();
    }
}
void MainWindow::playSelectedSong(QListWidgetItem *item)
{
    if (!item) return;  // Ensure an item is selected

    QString selectedSongPath = item->data(Qt::UserRole).toString();  // Retrieve stored path

    if (!selectedSongPath.isEmpty()) {
        qDebug() << "Playing selected song: " << selectedSongPath;
        mediaPlayer->setSource(QUrl::fromLocalFile(selectedSongPath));  // Set the selected song
        mediaPlayer->play();  // Play the song
    }
}
