# ProjectOpener (C++ / Win32)

## What this does

- Shows a simple dialog-like window with a selectable project list.
- For each project folder, searches only that folder (non-recursive) for the first `.code-workspace` file.
- If found, opens that workspace in VS Code with `code --reuse-window`.
- If not found, opens the folder path directly in VS Code.


## Build (Visual Studio 2026 Community)

Visual Studio 2026 Community Edition is free for individual developers, students, and open-source projects.

From the workspace root:

```powershell
cmake -S . -B build -G "Visual Studio 18 2026"
cmake --build build --config Release
```

Run:

```powershell
.\build\Release\ProjectOpener.exe
```

## Learning ideas

- Add a status area showing whether each path exists.
- Add keyboard shortcuts (Enter to open, Esc to quit).
- Replace raw Win32 UI with a framework later (Qt, wxWidgets, WinUI) after understanding this baseline.
