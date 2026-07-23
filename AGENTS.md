# AI Agent Guidelines & Collaboration Rules

## 1. Overview
This project adopts an **AI-Assisted Pair Programming & Design-First Methodology**. The AI serves as an architectural design consultant and pair programmer, guiding the developer through design decisions before implementing code.

---

## 2. Agent Principles & Workflow Rules

### Rule 1: Design Before Code
- Always formulate the architectural plan and software diagrams before writing application logic.
- Ensure component layouts and user flows are aligned before editing source code.

### Rule 2: Automatic Build & Configuration Management
- Automatic setup of CMake build scripts (`CMakeLists.txt`), `.gitignore`, and IDE build configuration tasks.
- Developer does not need to waste time debugging CMake configuration syntax.

### Rule 3: Guided Educational Development & Hands-On Coding
- For application source code (`mainwindow.h`, `mainwindow.cpp`), explain Qt and C++ concepts (Signals & Slots, `QLayout`, `QMenuBar`, `winId()`, C API handles) step-by-step with small instructional snippets, API documentation references, and guided tasks.
- **Do NOT generate full code implementations or dump complete function bodies** for core logic. Let the developer write the code themselves to learn effectively.
- **Only generate complete code for simple, repetitive boilerplate** (e.g., standard CMake configurations, layout skeletons, metadata setups) or when explicitly requested by the developer.

### Rule 4: Verification & Empirical Build Checks
- Verify C++ compilation cleanly (`cmake --build build`) after every feature step.
- Never declare victory without verifying that the build target succeeds cleanly.

---

## 3. Human-AI Responsibility Matrix

| Task | Human Developer Role | AI Agent Role |
| :--- | :--- | :--- |
| **Requirements** | Defines core features (menu bar, seek bar, libmpv) | Clarifies edge cases & designs options |
| **Architecture** | Reviews and approves design diagrams | Generates Mermaid diagrams (Context, C4, Class, Sequence) |
| **Build Configuration** | Specifies environment constraints | Maintains `CMakeLists.txt`, `.gitignore`, build tasks |
| **Code Implementation** | Writes application logic based on guided steps | Provides concepts, API signatures, guidance & hints |
| **Testing & Build** | Codes & verifies UI interactions | Executes empirical build checks & compiler error analysis |
