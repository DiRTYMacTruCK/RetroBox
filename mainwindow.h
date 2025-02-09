#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVector>
#include <QListWidgetItem>
#include <QLabel>
#include <QTreeWidget>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT  // ✅ Ensure this macro is here

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();
    void on_actionOpenLibrary_triggered();
    void on_metaDataChanged();
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void playNext();
    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void on_playButton_clicked();
    void on_stopButton_clicked();
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_actionAboutRetroBox_triggered();
    void on_treeItemClicked(QTreeWidgetItem *item, int column);
    void on_treeItemDoubleClicked(QTreeWidgetItem *item, int column);
    void playSelectedSong(QTreeWidgetItem *item, int column);  // ✅ Ensure function matches definition

private:  // ✅ Ensure `isPausedManually` is correctly placed in `private:`
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QVector<QString> trackList;
    int currentTrackIndex = 0;
    QLabel *nowPlayingLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
    QLabel *albumLabel;
    QLabel *yearLabel;
    QSlider *progressBar;
    QSlider *volumeSlider;
    QTreeWidget *treeWidget;
    bool isPausedManually = false;  // ✅ Correct placement (outside `private slots:`)
};

#endif // MAINWINDOW_H
