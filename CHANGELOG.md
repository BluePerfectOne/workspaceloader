# Changelog

All notable changes to this project are listed in this file.

## [0.4.0] - 2026-03-19

### Added in 0.4.0

- Show build timestamp in the `About` dialog.
- Show absolute paths for `projects.json`, `CHANGELOG.md`, and workspace folder in the `About` dialog.
- Add an `Open Workspace Folder` action from the `About` dialog.

### Changed in 0.4.0

- Move VS Code/CMake development environment setup and troubleshooting into `doc/docs/development-environment.md` and keep `README.md` concise.
- Document a clean command-line rehearsal workflow (`build-rehearsal`) with exact configure/build commands.

## [0.3.0] - 2026-03-19

### Added in 0.3.0

- Add an `About` dialog showing the current application version.
- Add an `Open Changelog` action from the `About` dialog.

## [0.2.0] - 2026-03-19

### Added in 0.2.0

- Open selected project path in File Explorer using the `Explorer` button.
- Support `Ctrl + double-click` on a project to open its folder in File Explorer.
- Show the application version in the main window title.

### Changed in 0.2.0

- Define the application version in CMake so the UI and documentation use the same version source.

## [0.1.0] - 2026-03-19

- Initial Qt-based project launcher window.
- Open selected workspace or folder in VS Code.
- Refresh project list without restarting the app.
- Keep window on top toggle.
- Replace raw Win32 UI with a framework later (Qt, wxWidgets, WinUI) after understanding this baseline. - replaced Win32 API with Qt
- Solved the Qt topmost flickering replacing that part with Win32 API functionality