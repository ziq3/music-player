#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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
private:
    void showEvent(QShowEvent *event) override;

    bool isPlaying = false;
    mpv_handle *mpv=nullptr;
    void initMpv();


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

    QVBoxLayout *mainLayout;
    QHBoxLayout *controlBarLayout;
    QHBoxLayout *seekBarLayout;



};
#endif
