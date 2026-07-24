#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "databasemanager.h"
#include <QMainWindow>
#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <mpv/client.h>
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void handleMpvEvents();

private:
    DatabaseManager dbManager;
    void showEvent(QShowEvent *event) override;

    bool isPlaying = false;
    bool isFileLoaded = false;
    mpv_handle *mpv = nullptr;
    void initMpv();
    QString formatTime(double seconds) const;

    void createMenuBar();
    void createCentralWidget();

    void createVideoArea();
    void createSeekBar();
    void createControlBar();
    void setupConnections();

    QWidget *centralWidget;

    QMenuBar *mainMenuBar;
    QMenu *fileMenu;
    QAction *openAction;

    QWidget *videoWidget;

    QLabel *currentTimeLabel;
    QSlider *seekSlider;
    QLabel *durationLabel;

    QPushButton *previousButton;
    QPushButton *playButton;
    QPushButton *nextButton;
    QPushButton *shuffle;
    QPushButton *loop;

    QVBoxLayout *mainLayout;
    QHBoxLayout *controlBarLayout;
    QHBoxLayout *seekBarLayout;
};
#endif
