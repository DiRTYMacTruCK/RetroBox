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
#include <QTimer>

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

    // Manually setting the geometry
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

    // ✅ Media Player & UI Button Connections
    connect(mediaPlayer, &QMediaPlayer::metaDataChanged, this, &MainWindow::on_metaDataChanged);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::on_mediaStatusChanged);
    connect(mediaPlayer, &QMediaPlayer::positionChanged, this, &MainWindow::on_positionChanged);
    connect(mediaPlayer, &QMediaPlayer::durationChanged, this, &MainWindow::on_durationChanged);
    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::on_playButton_clicked);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::on_stopButton_clicked);
    connect(ui->nextButton, &QPushButton::clicked, this, &MainWindow::on_nextButton_clicked);
    connect(ui->prevButton, &QPushButton::clicked, this, &MainWindow::on_prevButton_clicked);
    connect(ui->actionAboutRetroBox, &QAction::triggered, this, &MainWindow::on_actionAboutRetroBox_triggered);
    connect(ui->listArtists, &QListWidget::itemClicked, this, &MainWindow::on_artistSelected);
    connect(ui->listAlbums, &QListWidget::itemClicked, this, &MainWindow::on_albumSelected);
    connect(ui->listTracks, &QListWidget::itemDoubleClicked, this, &MainWindow::on_trackSelected);

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

void MainWindow::on_actionAboutRetroBox_triggered()
{
    QString aboutText =
        "<html><body><div style='text-align: center;'>"
        "<img src=':/resources/retrobox_v1.png' width='150' height='150'/>"
        "<h2>RetroBox</h2>"
        "<p><b>Version:</b> 0.1.6</p>"
        "<p><b>Build Date:</b> " __DATE__ " " __TIME__ "</p>"
        "<p><b>Developer:</b> DiRTY</p>"
        "<p>RetroBox is an open-source music player built with Qt.</p>"
        "<p><a href='https://github.com/dirtymactruck/RetroBox'>GitHub Repository</a></p>"
        "</div></body></html>";
    QMessageBox msgBox;
    msgBox.setWindowTitle("About RetroBox");
    msgBox.setTextFormat(Qt::RichText);
    msgBox.setText(aboutText);
    msgBox.exec();
}

void MainWindow::on_actionOpenLibrary_triggered()
{
    QString directoryPath = QFileDialog::getExistingDirectory(this, tr("Open Library"), "",
                                                              QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (directoryPath.isEmpty()) return;

    ui->listArtists->clear();  // ✅ Clear existing list
    trackList.clear();
    musicLibrary.clear();

    QDirIterator it(directoryPath, {"*.mp3", "*.wav", "*.flac", "*.ogg"},
                    QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();

        // ✅ Use a temporary QMediaPlayer to load metadata
        QMediaPlayer tempPlayer;
        tempPlayer.setSource(QUrl::fromLocalFile(filePath));

        // ✅ Allow Qt time to process metadata asynchronously
        QEventLoop loop;
        connect(&tempPlayer, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
        QTimer::singleShot(50, &loop, &QEventLoop::quit);  // Wait at most 50ms
        loop.exec();

        // ✅ Extract metadata
        QString artist = tempPlayer.metaData().value(QMediaMetaData::ContributingArtist).toStringList().join(", ");
        QString album = tempPlayer.metaData().value(QMediaMetaData::AlbumTitle).toString();
        QString title = tempPlayer.metaData().value(QMediaMetaData::Title).toString();

        // ✅ Fallbacks if metadata is empty
        if (artist.isEmpty()) artist = "Unknown Artist";
        if (album.isEmpty()) album = "Unknown Album";
        if (title.isEmpty()) title = fileName;

        // ✅ Store in hierarchical map
        musicLibrary[artist][album].append(filePath);
    }

    for (const QString &artist : musicLibrary.keys()) {
        ui->listArtists->addItem(artist);
    }

    qDebug() << "🎵 Library loaded successfully with correct metadata!";
}

void MainWindow::on_playButton_clicked()
{
    if (mediaPlayer->playbackState() == QMediaPlayer::PlayingState) {
        mediaPlayer->pause();
        qDebug() << "⏸ Playback paused.";
        ui->playButton->setText("▶ Play");
    }
    else if (mediaPlayer->playbackState() == QMediaPlayer::PausedState) {
        mediaPlayer->play();
        qDebug() << "▶ Resumed playback.";
        ui->playButton->setText("⏸ Pause");
    }
    else {
        QListWidgetItem *selectedTrack = ui->listTracks->currentItem();
        if (!selectedTrack) {
            qDebug() << "❌ No track selected!";
            return;
        }

        on_trackSelected(selectedTrack);  // ✅ Load and play the selected track
        mediaPlayer->play();  // ✅ Start playback
        qDebug() << "▶ Playing selected track.";
    }
}

void MainWindow::on_stopButton_clicked()
{
    mediaPlayer->stop();
    qDebug() << "⏹ Playback stopped.";
}

void MainWindow::on_nextButton_clicked()
{
    if (ui->listTracks->count() == 0) {
        qDebug() << "❌ No tracks to play.";
        return;
    }

    int nextIndex = ui->listTracks->currentRow() + 1;
    if (nextIndex >= ui->listTracks->count()) {
        nextIndex = 0;  // Loop back to first track
    }

    ui->listTracks->setCurrentRow(nextIndex);
    on_trackSelected(ui->listTracks->currentItem());
}

void MainWindow::on_prevButton_clicked()
{
    if (ui->listTracks->count() == 0) {
        qDebug() << "❌ No tracks to play.";
        return;
    }

    int prevIndex = ui->listTracks->currentRow() - 1;
    if (prevIndex < 0) {
        prevIndex = ui->listTracks->count() - 1;  // Loop back to last track
    }

    ui->listTracks->setCurrentRow(prevIndex);
    on_trackSelected(ui->listTracks->currentItem());
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
    if (mediaPlayer->metaData().isEmpty()) return;
    titleLabel->setText("Title: " + mediaPlayer->metaData().value(QMediaMetaData::Title).toString());
    artistLabel->setText("Artist: " + mediaPlayer->metaData().value(QMediaMetaData::ContributingArtist).toString());
    albumLabel->setText("Album: " + mediaPlayer->metaData().value(QMediaMetaData::AlbumTitle).toString());
}

void MainWindow::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia) {
        playNext();
    }
}

void MainWindow::playNext()
{
    qDebug() << "Playing next track...";
}

void MainWindow::on_artistSelected(QListWidgetItem *item)
{
    if (!item) return;

    QString artist = item->text();
    qDebug() << "🎶 Selected Artist: " << artist;  // Debugging output

    ui->listAlbums->clear();  // ✅ Clear previous albums
    ui->listTracks->clear();  // ✅ Clear previous tracks

    // ✅ Ensure the artist exists in musicLibrary before accessing
    if (musicLibrary.contains(artist)) {
        for (const QString &album : musicLibrary[artist].keys()) {
            ui->listAlbums->addItem(album);
        }
    } else {
        qDebug() << "❌ Artist not found in musicLibrary!";
    }
}
void MainWindow::on_albumSelected(QListWidgetItem *item)
{
    if (!item) return;

    QString album = item->text();
    QString artist = ui->listArtists->currentItem()->text(); // Get selected artist

    qDebug() << "📀 Selected Album: " << album << " by " << artist;

    ui->listTracks->clear();  // ✅ Clear previous track list

    // ✅ Ensure album exists in library
    if (musicLibrary.contains(artist) && musicLibrary[artist].contains(album)) {
        QVector<QPair<int, QString>> sortedTracks;  // ✅ Store (trackNumber, filePath)

        for (const QString &track : musicLibrary[artist][album]) {
            QMediaPlayer tempPlayer;
            tempPlayer.setSource(QUrl::fromLocalFile(track));

            QEventLoop loop;
            connect(&tempPlayer, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
            QTimer::singleShot(50, &loop, &QEventLoop::quit);
            loop.exec();

            int trackNumber = tempPlayer.metaData().value(QMediaMetaData::TrackNumber).toInt();
            if (trackNumber == 0) trackNumber = 9999; // ✅ Default if track number missing
            sortedTracks.append(qMakePair(trackNumber, track));
        }

        // ✅ Sort tracks by track number
        std::sort(sortedTracks.begin(), sortedTracks.end(),
                  [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
                      return a.first < b.first;
                  });

        // ✅ Populate sorted track list
        for (const auto &pair : sortedTracks) {
            QListWidgetItem *trackItem = new QListWidgetItem(QFileInfo(pair.second).fileName());
            trackItem->setData(Qt::UserRole, pair.second);  // ✅ Store file path in item
            ui->listTracks->addItem(trackItem);
            qDebug() << "🎵 Added Track: " << QFileInfo(pair.second).fileName() << " (#" << pair.first << ")";
        }
    } else {
        qDebug() << "❌ Album not found in musicLibrary!";
    }
}

void MainWindow::on_trackSelected(QListWidgetItem *item)
{
    if (!item) return;

    QString filePath = item->data(Qt::UserRole).toString();
    if (!filePath.isEmpty()) {
        qDebug() << "🎵 Track selected: " << filePath;

        mediaPlayer->setSource(QUrl::fromLocalFile(filePath));  // ✅ Load file into media player

        // ✅ Extract metadata for display
        QMediaPlayer tempPlayer;
        tempPlayer.setSource(QUrl::fromLocalFile(filePath));

        QEventLoop loop;
        connect(&tempPlayer, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
        QTimer::singleShot(50, &loop, &QEventLoop::quit);
        loop.exec();

        QString title = tempPlayer.metaData().value(QMediaMetaData::Title).toString();
        QString artist = tempPlayer.metaData().value(QMediaMetaData::ContributingArtist).toString();
        QString album = tempPlayer.metaData().value(QMediaMetaData::AlbumTitle).toString();
        QString year;

        if (!tempPlayer.metaData().value(QMediaMetaData::Date).isNull()) {
            year = tempPlayer.metaData().value(QMediaMetaData::Date).toDate().toString("yyyy");
        }

        // ✅ Fallbacks for missing metadata
        if (title.isEmpty()) title = QFileInfo(filePath).fileName();
        if (artist.isEmpty()) artist = "Unknown Artist";
        if (album.isEmpty()) album = "Unknown Album";
        if (year.isEmpty()) year = "Unknown Year";

        // ✅ Update "Now Playing" section
        nowPlayingLabel->setText("Now Playing: " + title);
        titleLabel->setText("Title: " + title);
        artistLabel->setText("Artist: " + artist);
        albumLabel->setText("Album: " + album);
        yearLabel->setText("Year: " + year);

        // ✅ Play the track if the player is already in play mode
        if (mediaPlayer->playbackState() != QMediaPlayer::PlayingState) {
            mediaPlayer->play();
            qDebug() << "▶ Auto-playing after selection.";
        }
    } else {
        qDebug() << "❌ No file path found for selected track!";
    }
}


