#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
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
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void openSettings();
    void on_metaDataChanged();
    void fetchMetadata(const QString &filePath, QString &title, QString &artist, QString &album);

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *playlist;
};

#endif // MAINWINDOW_H
