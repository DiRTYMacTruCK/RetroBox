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
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::playSelectedSong);
    connect(ui->treeWidget, &QTreeWidget::itemClicked, this, &MainWindow::on_treeItemClicked);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::on_stopButton_clicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::on_nextButton_clicked);
    connect(ui->prevButton, &QPushButton::clicked, this, &MainWindow::on_prevButton_clicked);
    connect(ui->actionAboutRetroBox, &QAction::triggered, this, &MainWindow::on_actionAboutRetroBox_triggered);
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::on_treeItemDoubleClicked);

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

    ui->treeWidget->clear();  // ✅ Clear existing list
    trackList.clear();

    QMap<QString, QMap<QString, QStringList>> musicLibrary; // ✅ Organized as Artist → Album → Tracks

    QDirIterator it(directoryPath, {"*.mp3", "*.wav", "*.flac", "*.ogg"},
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();

        // Extract metadata
        QMediaPlayer tempPlayer;
        tempPlayer.setSource(QUrl::fromLocalFile(filePath));
        tempPlayer.metaDataChanged();  // Force metadata to load

        QString artist = tempPlayer.metaData().value(QMediaMetaData::ContributingArtist).toString();
        QString album = tempPlayer.metaData().value(QMediaMetaData::AlbumTitle).toString();
        QString title = tempPlayer.metaData().value(QMediaMetaData::Title).toString();

        // Fallback if metadata is empty
        if (artist.isEmpty()) artist = "Unknown Artist";
        if (album.isEmpty()) album = "Unknown Album";
        if (title.isEmpty()) title = fileName;

        // Store in hierarchical map
        musicLibrary[artist][album].append(filePath);
    }

    // ✅ Populate `QTreeWidget`
    for (const QString &artist : musicLibrary.keys()) {
        QTreeWidgetItem *artistItem = new QTreeWidgetItem(ui->treeWidget);
        artistItem->setText(0, artist);

        for (const QString &album : musicLibrary[artist].keys()) {
            QTreeWidgetItem *albumItem = new QTreeWidgetItem(artistItem);
            albumItem->setText(0, album);

            for (const QString &track : musicLibrary[artist][album]) {
                QTreeWidgetItem *trackItem = new QTreeWidgetItem(albumItem);
                trackItem->setText(0, QFileInfo(track).fileName());
                trackItem->setData(0, Qt::UserRole, track);  // Store file path in the item
            }
        }
    }
}

void MainWindow::on_treeItemClicked(QTreeWidgetItem *item, int column)
{
    if (!item) return;

    QString filePath = item->data(0, Qt::UserRole).toString(); // Retrieve stored path

    if (!filePath.isEmpty()) {
        qDebug() << "Selected song: " << filePath;
    }
}
void MainWindow::on_treeItemDoubleClicked(QTreeWidgetItem *item, int column)
{
    if (!item) return;  // Ensure an item is selected

    QString filePath = item->data(0, Qt::UserRole).toString();  // Retrieve stored path

    if (!filePath.isEmpty()) {
        qDebug() << "Playing selected song: " << filePath;
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));  // Set the selected song
        mediaPlayer->play();  // Play the song
    }
}
void MainWindow::on_playButton_clicked()
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        isPausedManually = true;  // ✅ Mark as manually paused
        mediaPlayer->pause();
        qDebug() << "⏸ Playback paused at position:" << mediaPlayer->position();
        ui->playButton->setText("▶ Play");
    }
    else {
        isPausedManually = false;  // ✅ Clear the flag when resuming
        mediaPlayer->play();
        qDebug() << "▶ Resuming playback from position:" << mediaPlayer->position();
        ui->playButton->setText("⏸ Pause");
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
void MainWindow::playSelectedSong(QTreeWidgetItem *item, int column)
{
    if (!item) return;  // Ensure an item is selected

    QString filePath = item->data(0, Qt::UserRole).toString();  // Retrieve stored path

    if (!filePath.isEmpty()) {
        qDebug() << "Playing selected song: " << filePath;
        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));  // Set the selected song
        mediaPlayer->play();  // Play the song
    }
}
void MainWindow::on_actionAboutRetroBox_triggered()
{
    QString aboutText =
        "<html>"
        "<body>"
        "<div style='text-align: center;'>"
        "<img src=':/resources/retrobox_v1.png' width='150' height='150'/>"  // ✅ Updated path
        "<h2>RetroBox</h2>"
        "<p><b>Version:</b> 0.1.2</p>"
        "<p><b>Build Date:</b> " __DATE__ " " __TIME__ "</p>"
        "<p><b>Developer:</b>DiRTY</p>"
        "<p>RetroBox is an open-source music player built with Qt.</p>"
        "<p><a href='https://github.com/dirtymactruck/RetroBox'>GitHub Repository</a></p>"
        "</div>"
        "</body>"
        "</html>";

    QMessageBox msgBox;
    msgBox.setWindowTitle("About RetroBox");
    msgBox.setTextFormat(Qt::RichText);  // ✅ Enables HTML rendering
    msgBox.setText(aboutText);
    msgBox.exec();
}
