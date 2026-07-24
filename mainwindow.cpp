#include "mainwindow.h"

#include <QAction>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QTime>
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{

    createMenuBar();
    createCentralWidget();
    createVideoArea();
    createSeekBar();
    createControlBar();

    setupConnections();
}
void MainWindow::initMpv()
{
    mpv = mpv_create();
    if (!mpv)
        return;
    intptr_t wid = videoWidget->winId();
    mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &wid);
    mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
    mpv_initialize(mpv);
    mpv_set_wakeup_callback(
            mpv,
            [](void *ctx) {
                QMetaObject::invokeMethod(static_cast<MainWindow *>(ctx),
                                          &MainWindow::handleMpvEvents, Qt::QueuedConnection);
            },
            this);
}
QString MainWindow::formatTime(double seconds) const
{
    int secs = static_cast<int>(seconds);
    return QTime(0, 0).addSecs(secs).toString(secs >= 3600 ? "hh:mm:ss" : "mm:ss");
}
void MainWindow::createCentralWidget()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainLayout = new QVBoxLayout(centralWidget);
}

void MainWindow::createMenuBar()
{
    mainMenuBar = menuBar();
    fileMenu = mainMenuBar->addMenu("File");
    openAction = fileMenu->addAction("Open...");
}

void MainWindow::createVideoArea()
{
    videoWidget = new QWidget(centralWidget);
    videoWidget->setStyleSheet("background-color: black;");
    mainLayout->addWidget(videoWidget, 1);
}

void MainWindow::createControlBar()
{
    controlBarLayout = new QHBoxLayout;
    mainLayout->addLayout(controlBarLayout);

    previousButton = new QPushButton("Prev");
    playButton = new QPushButton("Play");
    nextButton = new QPushButton("Next");
    shuffle = new QPushButton("Shuffle");
    loop = new QPushButton("Loop");

    controlBarLayout->addWidget(previousButton);
    controlBarLayout->addWidget(playButton);
    controlBarLayout->addWidget(nextButton);
    controlBarLayout->addWidget(shuffle);
    controlBarLayout->addWidget(loop);
}
void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (!mpv) {
        initMpv();
    }
}
void MainWindow::createSeekBar()
{
    seekBarLayout = new QHBoxLayout;
    mainLayout->addLayout(seekBarLayout);

    currentTimeLabel = new QLabel("00:00");
    seekSlider = new QSlider(Qt::Horizontal);
    durationLabel = new QLabel("00:00");

    seekBarLayout->addWidget(currentTimeLabel);
    seekBarLayout->addWidget(seekSlider);
    seekBarLayout->addWidget(durationLabel);
}

void MainWindow::setupConnections()
{
    connect(openAction, &QAction::triggered, this, [this] {
        QString fileName =
                QFileDialog::getOpenFileName(this, "Open File", QString(), "All Files (*)");

        if (!fileName.isEmpty()) {
            setWindowTitle("Player - " + fileName);
            const char *cmd[] = { "loadfile", fileName.toUtf8().constData(), NULL };
            mpv_command(mpv, cmd);
        }
    });

    connect(playButton, &QPushButton::clicked, this, [this] {
        if (!isFileLoaded)
            return;
        if (isPlaying) {
            playButton->setText("Play");
        } else {
            playButton->setText("Pause");
        }
        const char *cmd[] = { "cycle", "pause", NULL };
        mpv_command(mpv, cmd);
        isPlaying = !isPlaying;
    });
    connect(seekSlider, &QSlider::sliderReleased, this, [this] {
        if (!isFileLoaded)
            return;
        if (mpv) {
            int pos = seekSlider->value();
            QString posStr = QString::number(pos);

            const char *cmd[] = { "seek", posStr.toUtf8().constData(), "absolute", NULL };
            mpv_command(mpv, cmd);
        }
    });
}
MainWindow::~MainWindow()
{
    if (mpv) {
        mpv_terminate_destroy(mpv);
        mpv = nullptr;
    }
}
void MainWindow::handleMpvEvents()
{
    while (mpv) {
        mpv_event *event = mpv_wait_event(mpv, 0);
        if (event->event_id == MPV_EVENT_NONE)
            break;
        if (event->event_id == MPV_EVENT_FILE_LOADED) {
            isPlaying = true;
            isFileLoaded = true;
            playButton->setText("Pause");
            double durationInSeconds = 0.0;
            if (mpv_get_property(mpv, "duration", MPV_FORMAT_DOUBLE, &durationInSeconds) == 0) {

                seekSlider->setMaximum(static_cast<int>(durationInSeconds));
                durationLabel->setText(formatTime(durationInSeconds));
            }
        }
        if (event->event_id == MPV_EVENT_PROPERTY_CHANGE) {
            mpv_event_property *mpv_property = static_cast<mpv_event_property *>(event->data);
            if (mpv_property->data) {
                if (QString(mpv_property->name) == "time-pos") {
                    double seconds = *static_cast<double *>(mpv_property->data);
                    currentTimeLabel->setText(formatTime(seconds));
                    if (!seekSlider->isSliderDown()) {
                        seekSlider->setValue(static_cast<int>(seconds));
                    }
                }
            }
        }
    }
}