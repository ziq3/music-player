# AI Agent Guidelines & Collaboration Rules

## 1. Overview
This project adopts an **AI-Assisted Pair Programming & Design-First Methodology**. The human developer acts as the lead software architect and developer who formulates system designs, SQLite schemas, and core application logic. The AI serves as an architectural consultant and technical mentor, assisting with design trade-offs, diagram generation, build maintenance, and guided C++ Qt/SQLite concepts.

---

## 2. Core Principles & Workflow Rules

### Rule 1: Design Before Code (Architecture & Schema First)
- The human developer designs the architectural plan, SQLite database schema, data flow, and software structure before implementing application logic.
- AI assists in visualizing and maintaining software diagrams in `docs/DESIGN.md` (Context, C4, Data Flow, ERD, Class, and Sequence diagrams).

### Rule 2: Automatic Build & Configuration Management
- Automatic setup and maintenance of CMake build scripts (`CMakeLists.txt`), `.gitignore`, and IDE build configuration tasks.
- The human developer focuses on core domain logic while the AI manages boilerplate build configurations and compiler flag setups.

### Rule 3: Guided Educational Development & Hands-On Coding
- **Concepts & Guided Hints**: For application source code (`MainWindow`, `DatabaseManager`, `PlaylistManager`, `TrackTableModel`), the AI explains C++17 and Qt concepts step-by-step (Signals & Slots, Model/View separation, `QAbstractTableModel`, `winId()` handle embedding, SQLite WAL mode) with small instructional snippets and API references.
- **Hands-On Developer Implementation**: **Do NOT generate full code dumps or overwrite core logic function bodies** unless explicitly requested. Let the developer implement core functions to learn effectively.
- **Boilerplate Exemption**: The AI provides complete code for repetitive boilerplate (e.g. CMake configurations, skeleton headers, metadata setups).

### Rule 4: Empirical Build Verification & Non-Functional Checks
- Execute empirical compilation checks (`cmake --build build`) after every module step.
- Verify non-functional targets: sub-10s cold startup over 8,000 FMA tracks, zero UI freezing, and SQLite transaction durability (`SIGKILL` persistence).

---

## 3. Human-AI Responsibility Matrix

| Task Area | Human Developer Role | AI Agent Role |
| :--- | :--- | :--- |
| **Requirements & UX** | Defines feature goals, UX flows & layout preferences | Clarifies edge cases, performance targets & layout options |
| **System Architecture & Database Schema** | **Designs, formulates & approves** software architecture and SQLite schemas | Proposes architectural trade-offs & generates Mermaid diagrams (Context, C4, Flow, ERD, Class, Sequence) |
| **Build Configuration** | Specifies platform environment (Ubuntu 20.04/22.04/24.04, C++17, Qt6) | Maintains `CMakeLists.txt`, `.gitignore`, and build steps |
| **Data Ingestion** | Defines `tracks`, `playlists` schema & CLI parameters for `seed.py` | Provides Python script guidance, batch SQL snippets & CLI parser structure |
| **Core Logic Coding** | Implements function bodies for database, models, and player controls | Provides Qt/C++ concepts, API signatures, guidance & instructional snippets |
| **Testing & Quality** | Verifies UI interactions and audio playback in runtime | Executes empirical build checks & compiler error analysis |

---

## 4. Prompting & AI Collaboration Strategy for Evaluators

1. **Architectural Ideation**: Use AI to explore trade-offs between Qt Multimedia vs. `libmpv`, and in-memory lists vs. SQLite `QAbstractTableModel`.
2. **Step-by-Step Guidance**: The AI acts as a senior technical mentor, breaking down complex C++ Qt concepts into manageable learning steps.
3. **Iterative Refinement**: Small, focused commits with real commit history, verifying clean compilation at each stage.
