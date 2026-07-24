# Architecture & Software Design Document

## 1. Overview
This document outlines the software design and architecture of the **FMA Desktop Music Player** application built using **Qt 6 (C++)**, **libmpv**, and **SQLite**. The system is designed to provide high-performance local browsing and playback of the **FMA Small Dataset (8,000 tracks)** with sub-second responsiveness, playlist management, shuffle, loop, and instant search capabilities.

---

## 2. System Context Diagram (`context`)
Visualizes the external dataset, seed pipeline, application components, and operating system dependencies.

```mermaid
graph TD
    FMADataset["📁 FMA Small Dataset (tracks.csv + MP3 files)"] -->|"Seed Ingestion CLI"| SeedScript["🐍 Python Seed Script (seed.py)"]
    SeedScript -->|"Populates Metadata & File Paths"| SQLiteDB[("💾 SQLite Database (music.db)")]
    
    User["👤 User"] -->|"Interacts with UI (Browse, Play, Playlist, Seek, Search)"| QtApp["🎵 Qt Desktop Music Player (C++ App)"]
    
    QtApp -->|"Queries Tracks & Playlists (Indexed)"| SQLiteDB
    QtApp -->|"Audio Decoding & Playback Commands"| LibMPV["🎬 libmpv Engine (C API)"]
    LibMPV -->|"Stream Audio Data"| OS["🖥️ OS Audio System (ALSA/PulseAudio/PipeWire)"]
```

---

## 3. C4 Component Diagram (`C4`)
Details the internal component architecture of the Qt C++ application.

```mermaid
graph TB
    subgraph Qt Desktop Application ["Qt Desktop Music Player (C++)"]
        subgraph UI Layer ["UI Components"]
            MainWindow["MainWindow (Central Container)"]
            Sidebar["Sidebar Navigation (Library / Playlists)"]
            TrackView["Track Table View (QTableView + Custom Model)"]
            PlaylistView["Playlist Manager View (Ordering / Reordering)"]
            SearchBar["Search / Filter Bar (QLineEdit)"]
            PlayerBar["Playback Controls Bar (Play/Pause, Seek, Shuffle, Loop)"]
            
            MainWindow --> Sidebar
            MainWindow --> TrackView
            MainWindow --> PlaylistView
            MainWindow --> SearchBar
            MainWindow --> PlayerBar
        end

        subgraph Business Logic Layer ["Core Application Logic"]
            DBManager["DatabaseManager (SQLite Connection & Transactions)"]
            PlaylistManager["PlaylistManager (Shuffle, Reordering, Loop Modes)"]
            MPVController["MpvController (libmpv C API wrapper)"]
        end

        TrackView -->|"Fetch Rows (Virtual Model)"| DBManager
        PlaylistView -->|"Persist Order / Items"| PlaylistManager
        PlaylistManager -->|"Store Playlist State"| DBManager
        PlayerBar -->|"Trigger Play/Pause/Seek/Next"| MPVController
        MPVController -->|"Playback Status Signals"| PlayerBar
    end

    subgraph Database Layer ["Persistence"]
        SQLiteFile[("music.db (WAL Mode)")]
    end

    DBManager -->|"Prepared SQL Queries"| SQLiteFile
```

---

## 4. Data Flow Diagram (`flow`)
Illustrates how the FMA dataset is ingested into the SQLite store and accessed by the client.

```mermaid
flowchart LR
    A["tracks.csv (Metadata)"] -->|Parse & Clean| B["seed.py CLI"]
    C["fma_small/ (8,000 MP3s)"] -->|Match Track IDs| B
    B -->|Batch INSERT with Transactions| D[("SQLite music.db")]
    D -->|PRAGMA journal_mode=WAL| E["Fast Startup (< 10s)"]
    E -->|QAbstractTableModel| F["Qt App UI Rendering"]
```

---

## 5. Entity-Relationship Diagram (`ERD`)
Defines the relational SQLite database schema.

```mermaid
erDiagram
    TRACKS {
        INTEGER track_id PK
        TEXT title
        TEXT artist_name
        TEXT album_title
        TEXT genre
        REAL duration
        TEXT file_path
    }

    PLAYLISTS {
        INTEGER playlist_id PK
        TEXT name
        TEXT created_at
    }

    PLAYLIST_TRACKS {
        INTEGER playlist_id FK
        INTEGER track_id FK
        INTEGER position
    }

    PLAYLISTS ||--o{ PLAYLIST_TRACKS : "contains"
    TRACKS ||--o{ PLAYLIST_TRACKS : "included in"
```

---

## 6. Class Diagram (`class`)
Describes the object-oriented structure of the Qt C++ application.

```mermaid
classDiagram
    class QMainWindow {
        <<Qt Base>>
    }

    class QAbstractTableModel {
        <<Qt Base>>
    }

    class MainWindow {
        -DatabaseManager *dbManager
        -PlaylistManager *playlistManager
        -TrackTableModel *trackModel
        -QTableView *trackTableView
        -QLineEdit *searchLineEdit
        -QPushButton *playButton
        -QPushButton *prevButton
        -QPushButton *nextButton
        -QPushButton *shuffleButton
        -QPushButton *loopButton
        -QSlider *seekSlider
        -mpv_handle *mpv
        +MainWindow(QWidget *parent)
        +~MainWindow()
        -void initUi()
        -void initMpv()
        -void setupConnections()
    }

    class DatabaseManager {
        -QSqlDatabase db
        +DatabaseManager(QString dbPath)
        +bool open()
        +QList~Track~ getAllTracks()
        +QList~Track~ searchTracks(QString query)
        +QList~Playlist~ getPlaylists()
        +bool createPlaylist(QString name)
        +bool addTrackToPlaylist(int playlistId, int trackId)
        +bool reorderPlaylistTrack(int playlistId, int fromPos, int toPos)
    }

    class TrackTableModel {
        -QList~Track~ tracks
        +int rowCount()
        +int columnCount()
        +QVariant data(QModelIndex, int role)
        +QVariant headerData(...)
        +void setTracks(QList~Track~ newTracks)
    }

    class PlaylistManager {
        -QList~Track~ currentQueue
        -int currentIndex
        -bool isShuffle
        -LoopMode loopMode
        +void setQueue(QList~Track~ queue)
        +Track getCurrentTrack()
        +Track getNextTrack()
        +Track getPreviousTrack()
        +void toggleShuffle()
        +void cycleLoopMode()
    }

    QMainWindow <|-- MainWindow
    QAbstractTableModel <|-- TrackTableModel
    MainWindow --> DatabaseManager
    MainWindow --> PlaylistManager
    MainWindow --> TrackTableModel
```

---

## 7. Sequence Diagram (`sequence`)
Traces user events from clicking a song to audio decoding and reordering.

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant UI as MainWindow / TrackView
    participant PM as PlaylistManager
    participant DB as DatabaseManager
    participant MPV as libmpv Engine

    User->>UI: Select Track & Double-Click
    UI->>PM: setCurrentTrack(trackId)
    PM->>DB: getTrackDetails(trackId)
    DB-->>PM: Track metadata & file_path
    PM->>MPV: mpv_command("loadfile", filePath)
    MPV-->>UI: MPV_EVENT_FILE_LOADED signal
    UI->>UI: Update Track Title, Duration Slider, Play state

    User->>UI: Drag & Drop Playlist Track (Reorder)
    UI->>DB: reorderPlaylistTrack(playlistId, oldPos, newPos)
    DB->>DB: UPDATE playlist_tracks SET position = ...
    DB-->>UI: Success
    UI->>PM: updateQueueOrder()
```
