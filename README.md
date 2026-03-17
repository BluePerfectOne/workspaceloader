# ProjectOpener (C++ / Win32)

This example recreates your PowerShell launcher as a native Windows C++ desktop app.

## What it does

- Shows a simple dialog-like window with a selectable project list.
- For each project folder, searches only that folder (non-recursive) for the first `.code-workspace` file.
- If found, opens that workspace in VS Code with `code --reuse-window`.
- If not found, opens the folder path directly in VS Code.

## Project list used

The app currently includes the same projects as your script:

- CCE Module -> `E:\Projects\Kemplink\CCE_Module\CCE_Module\`
- Ironlog (WSL2) -> `\\wsl$\Ubuntu-22.04\home\sami\projects\ironlog\`
- Helios (LV2021) -> `E:\Projects\AION-POWER\HELIOS\2.HELIOS_LV2021\PLATFORM_NEXT\`
- THOR (LV2017) -> `E:\Projects\Enedo\Latest\THOR04\PN_UTNG\`

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

- Move the project list into a JSON file and load it at startup.
- Add a status area showing whether each path exists.
- Add keyboard shortcuts (Enter to open, Esc to quit).
- Replace raw Win32 UI with a framework later (Qt, wxWidgets, WinUI) after understanding this baseline.
