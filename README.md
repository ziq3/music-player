# Qt 6 Music & Video Player (`libmpv`)

A modern desktop music and video player built with **Qt 6 (C++)** and an embedded **libmpv** media engine.

---

## 🚀 Features
- **Native Video Canvas**: Embedded video playback via `libmpv` and Qt `winId()`.
- **Playback Controls**: Play/Pause toggle, seek bar, time labels, and volume layout.
- **Menu Bar**: Native `QMenuBar` with open file dialog support.
- **Resource Safety**: Proper C API RAII resource teardown with `mpv_terminate_destroy()`.

---

## 🛠️ Project Structure
```text
my-project/
├── CMakeLists.txt     # Build configuration
├── AGENTS.md          # AI pair programming guidelines & rules
├── README.md          # Project overview & build instructions
├── docs/
│   └── DESIGN.md      # Architecture & Mermaid system diagrams
└── mainwindow.h/cpp   # Application source code
```

---

## 📦 Building & Running

### Prerequisites
- GCC / Clang (C++17)
- Qt 6 (`Qt6Widgets`)
- libmpv development library (`libmpv-dev`)
- CMake 3.16+

### Commands
```bash
# Configure build directory
cmake -B build -S .

# Build application binary
cmake --build build

# Run executable
./build/untitled1
```

---

## 📚 Documentation & Guidelines
- **Architecture & Software Design**: See [docs/DESIGN.md](file:///home/pc/untitled1/docs/DESIGN.md) for C4, Sequence, and Class diagrams.
- **AI Pair Programming Rules**: See [AGENTS.md](file:///home/pc/untitled1/AGENTS.md) for guided learning principles.
