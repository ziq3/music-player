#include "mainwindow.h"

#include <QAction>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createMenuBar();
    createCentralWidget();
    createVideoArea();
    createSeekBar();
    createControlBar();
    setupConnections();
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
    videoWidget = new QWidget(this);
    videoWidget->setStyleSheet("background-color: black;");
    mainLayout->addWidget(videoWidget, 1);
}

void MainWindow::createControlBar()
{
    controlBarLayout = new QHBoxLayout;
    mainLayout->addLayout(controlBarLayout);

    previousButton = new QPushButton("Prev", this);
    playButton = new QPushButton("Play", this);
    nextButton = new QPushButton("Next", this);

    controlBarLayout->addWidget(previousButton);
    controlBarLayout->addWidget(playButton);
    controlBarLayout->addWidget(nextButton);
}

void MainWindow::createSeekBar()
{
    seekBarLayout = new QHBoxLayout;
    mainLayout->addLayout(seekBarLayout);

    currentTimeLabel = new QLabel("00:00", this);
    seekSlider = new QSlider(Qt::Horizontal, this);
    durationLabel = new QLabel("22:00", this);

    seekBarLayout->addWidget(currentTimeLabel);
    seekBarLayout->addWidget(seekSlider);
    seekBarLayout->addWidget(durationLabel);
}

void MainWindow::setupConnections()
{
    connect(openAction, &QAction::triggered, this, [this] {
        QString fileName = QFileDialog::getOpenFileName(this, "Open File", QString(), "All Files (*.*)");
        if (!fileName.isEmpty()) {
            setWindowTitle("Player - " + fileName);
        }
    });

    connect(playButton,&QPushButton::clicked,this,[this]{
        if(isPlaying)
            {
            playButton->setText("Play");
        }
        else
            {
            playButton->setText("Pause");
        }
        isPlaying=!isPlaying;
    });
}
