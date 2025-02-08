#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QStringList>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionOpenLibrary_triggered();
    void on_playButton_clicked();
    void on_songSelected(QListWidgetItem *item);
    void on_metaDataChanged();
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QStringList trackList;  // Replaces QMediaPlaylist
    int currentTrackIndex;
};

#endif // MAINWINDOW_H
