#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVector>
#include <QListWidget>
#include <QLabel>
#include <QMap>
#include <QSlider>
#include <QAudioOutput>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionOpenLibrary_triggered();
    void on_artistSelected(QListWidgetItem *item);
    void on_albumSelected(QListWidgetItem *item);
    void on_trackSelected(QListWidgetItem *item);
    void on_playButton_clicked();
    void on_stopButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void on_metaDataChanged();
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playNext();
    void on_actionAboutRetroBox_triggered();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QMap<QString, QMap<QString, QStringList>> musicLibrary;
    QVector<QString> trackList;
    int currentTrackIndex = 0;
    QLabel *nowPlayingLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
    QLabel *albumLabel;
    QLabel *yearLabel;
    QSlider *progressBar;
    QSlider *volumeSlider;
    bool isPausedManually = false;
};

#endif // MAINWINDOW_H
