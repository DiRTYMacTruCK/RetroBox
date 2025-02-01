#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionOpenLibrary_triggered();
    void on_actionExit_triggered();
    void openSettings();
    void on_playButton_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *playlist;
};

#endif
