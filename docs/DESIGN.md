# Architecture & Software Design Document

## 1. Overview
This document outlines the software design and architecture of the **Qt Music & Video Player Application** built using **Qt 6 (C++)** and **libmpv**.

---

## 2. System Context Diagram
Visualizes how the user interacts with the application and external system dependencies.

```mermaid
graph TD
    User["👤 User"] -->|"Interacts with UI (Play, Open File, Seek)"| PlayerApp["🎵 Qt Music & Video Player (C++ App)"]
    PlayerApp -->|"Reads Media Files"| FileSystem["📁 Local File System"]
    PlayerApp -->|"Decodes & Renders Audio/Video"| LibMPV["🎬 libmpv Engine (C API)"]
    LibMPV -->|"Outputs Audio/Video"| OS["🖥️ OS Audio/Display Drivers"]
```

---

## 3. C4 Component Diagram
Details the internal components of the Qt application architecture.

```mermaid
graph TB
    subgraph Qt App ["Qt Music & Video Player"]
        UI["🖥️ UI Component (MainWindow)"]
        MenuBar["Horizontal Menu Bar (File -> Open)"]
        VideoArea["Video Container Widget (QWidget)"]
        SeekBar["Seek & Time Controls (QSlider, QLabel)"]
        ControlBar["Playback Buttons (Prev, Play/Pause, Next)"]
        
        UI --> MenuBar
        UI --> VideoArea
        UI --> SeekBar
        UI --> ControlBar
    end

    subgraph Core Logic ["Media Core & Event Handling"]
        SignalSlots["Qt Signals & Slots Event Engine"]
        MPVController["libmpv Controller (mpv_handle)"]
    end

    UI --> SignalSlots
    SignalSlots --> MPVController
    MPVController -->|"winId() Embedding & Commands"| LibMPVEngine["libmpv C API"]
```

---

## 4. User Interaction & Playback Flow
Illustrates the user workflow from launching the application to playing media.

```mermaid
stateDiagram-v2
    [*] --> Idle: Launch App
    Idle --> SelectingFile: Click 'File -> Open'
    SelectingFile --> LoadingFile: Choose Video/Audio File
    LoadingFile --> Playing: Load into libmpv
    
    state Playing {
        [*] --> VideoPlayback
        VideoPlayback --> Paused: Click 'Pause' Button
        Paused --> VideoPlayback: Click 'Play' Button
        VideoPlayback --> Seeking: Drag Seek Slider
        Seeking --> VideoPlayback: Release Slider
    }

    Playing --> Idle: Stop / Close App
```

---

## 5. Class Diagram
Describes the object-oriented structure of the Qt C++ application.

```mermaid
classDiagram
    class QMainWindow {
        <<Qt Base>>
    }

    class MainWindow {
        -bool isPlaying
        -QWidget *centralWidget
        -QMenuBar *mainMenuBar
        -QMenu *fileMenu
        -QAction *openAction
        -QWidget *videoWidget
        -QLabel *currentTimeLabel
        -QSlider *seekSlider
        -QLabel *durationLabel
        -QPushButton *previousButton
        -QPushButton *playButton
        -QPushButton *nextButton
        -mpv_handle *mpv
        +MainWindow(QWidget *parent)
        +~MainWindow()
        -void initMpv()
        -void createMenuBar()
        -void createCentralWidget()
        -void createVideoArea()
        -void createSeekBar()
        -void createControlBar()
        -void setupConnections()
    }

    QMainWindow <|-- MainWindow : Inherits
```

---

## 6. Event & Signal Sequence Diagram
Traces the sequence of calls when the user opens and plays a file.

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant MenuBar as QMenuBar / QAction
    participant MW as MainWindow
    participant Dialog as QFileDialog
    participant MPV as libmpv Engine
    participant VideoWidget as QWidget (videoWidget)

    User->>MenuBar: Click 'File -> Open...'
    MenuBar->>MW: trigger openAction signal
    MW->>Dialog: getOpenFileName(this, "Open File")
    Dialog-->>MW: Return selected filePath
    MW->>MPV: mpv_set_option("wid", videoWidget.winId())
    MW->>MPV: mpv_command("loadfile", filePath)
    MPV->>VideoWidget: Render Video Frames
    MW->>User: Update Window Title & Controls
```
