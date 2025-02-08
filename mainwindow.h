#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVector>  // Better for indexed playlist handling
#include <QListWidgetItem>
#include <QLabel>

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
    void on_playButton_clicked();
    void on_songSelected(QListWidgetItem *item);
    void on_metaDataChanged();
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playNext();  // Handles playing the next song manually

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QVector<QString> trackList;  // Replaces QMediaPlaylist with manual playlist handling
    int currentTrackIndex = 0;   // Ensure it's initialized

    QLabel *nowPlayingLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
    QLabel *albumLabel;
    QLabel *yearLabel;
};

#endif // MAINWINDOW_H
