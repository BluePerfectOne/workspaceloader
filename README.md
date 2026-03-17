# ProjectOpener (C++ / Win32)

## Use Case & Pain Point

When working with multiple projects across different programming languages and environments, keeping track of all the directories becomes increasingly difficult. While you could document project locations somewhere, it is easy to lose track or forget where things are. ProjectOpener solves this by providing a simple, always-on-top window that lists your projects and lets you quickly open or switch workspaces in VS Code. This tool keeps your project list accessible and visible, so you never have to hunt for directories or remember paths. It streamlines your workflow by making workspace switching effortless and always available.

## What this does

- Shows a simple dialog-like window with a selectable project list.
- For each project folder, searches only that folder (non-recursive) for the first `.code-workspace` file.
- If found, opens that workspace in VS Code with `code --reuse-window`.
- If not found, opens the folder path directly in VS Code.

## Example: Project List JSON

ProjectOpener uses a JSON file to define your project list. By default, this file should be named `projects.json` and placed in the working directory (the root folder where you launch ProjectOpener).

Each entry in the list should have a `name` (displayed in the UI) and a `path` (the directory to open). Example:

```json
[
  {
    "name": "Project_1 (STM32)",
    "path": "X:\\Projects\\CUSTOMER1\\PRODUCTNAME1\\"
  },
  {
    "name": "Project_2 (WSL2)",
    "path": "\\\\wsl$\\Ubuntu-22.04\\home\\username\\projects\\productname\\"
  },
  {
    "name": "Project_3 (Java)",
    "path": "X:\\Projects\\CUSTOMER2\\PRODUCTNAME2\\"
  },
  {
    "name": "Project_4 (LV2017)",
    "path": "X:\\Projects\\CUSTOMER3\\PRODUCTNAME3\\"
  },
  {
    "name": "ProjectLauncher (WIN11)",
    "path": "X:\\Projects\\Win\\ProjectOpener\\"
  }
]
```

- `name`: The label shown in the ProjectOpener window.
- `path`: The absolute or network path to the project directory.


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
