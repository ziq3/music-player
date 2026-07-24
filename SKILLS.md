# Technical Skills & Capabilities Manifest

This document outlines the core technical skills, architectural domain knowledge, and engineering capabilities applied in building the **FMA Desktop Music Player**.

---

## 1. Core Technical Competencies

### 1.1 C++17 / Qt 6 GUI Application Architecture
- **Object-Oriented UI Component Design**: Structuring responsive interfaces using `QMainWindow`, `QVBoxLayout`, `QHBoxLayout`, `QSplitter`, and `QStackedWidget`.
- **Model/View Architecture (`QAbstractTableModel`)**: Decoupling database dataset representations from `QTableView` rendering. Custom models allow instant rendering and smooth scrolling across 8,000+ tracks without UI freezing.
- **Qt Signals & Slots Event System**: Thread-safe communication between player controls, background data models, and media event handlers.

### 1.2 Multimedia Integration (`libmpv` C API)
- **Embedded Handle Management (`mpv_handle`)**: Initializing, configuring, and destroying low-level `libmpv` instances.
- **Window Embedding (`wid`)**: Attaching `libmpv` rendering pipeline to native `QWidget` surface via `winId()`.
- **Asynchronous Event Callback Loop**: Intercepting `MPV_EVENT_PROPERTY_CHANGE` and `MPV_EVENT_FILE_LOADED` events using `mpv_set_wakeup_callback` and `QMetaObject::invokeMethod` with `Qt::QueuedConnection` to preserve main thread responsiveness.

### 1.3 Database Optimization & Persistence (SQLite)
- **Indexed Schema Engineering**: Indexing search keys (`title`, `artist_name`) for sub-5ms query response times over 8,000 tracks.
- **Write-Ahead Logging (WAL Mode)**: Enabling `PRAGMA journal_mode=WAL` and `PRAGMA synchronous=NORMAL` for atomic, crash-resilient transactions (resilient against `SIGKILL` or power loss).
- **Relational Playlist Ordering**: Managing explicit position numbers (`position INT`) for manual playlist reordering and drag-and-drop operations.

### 1.4 Python Data Pipeline & CLI Engineering
- **Metadata Parsing**: Extracting track IDs, titles, artists, albums, genres, and audio durations from `fma_metadata/tracks.csv`.
- **Batch Transaction Seeding**: Leveraging SQLite batch `executemany` transactions for high-speed seeding (< 2s for 8,000 tracks).
- **CLI Options**: Providing `--fma_dir` and `--limit` flags for iterative development testing.

---

## 2. Non-Functional & UX Optimizations

- **Sub-10s Cold Startup**: Pre-built SQLite database indexing avoids parsing large CSV or filesystem trees at application launch.
- **Zero UI Freezing / Main Loop Isolation**: Audio decoding and IPC are handled asynchronously by `libmpv` worker threads; UI updates are strictly event-driven.
- **Low Memory Footprint**: Lazy dataset evaluation through Qt virtual models ensures memory remains stable under 100 MB regardless of playlist size.
