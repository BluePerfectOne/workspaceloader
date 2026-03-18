# Development Environment Setup (VS Code)

This project builds through CMake, but the VS Code CMake Tools extension still needs a correctly configured toolchain environment.

## Prerequisites

- Visual Studio 2026 Community with MSVC x64 build tools
- Windows 10/11 SDK with mt.exe and rc.exe
- CMake 3.16 or newer
- Qt 6.10.2 for MSVC at E:/Qt/6.10.2/msvc2022_64
- VS Code extensions:
  - CMake Tools
  - C/C++

## Workspace Configuration

The workspace file ProjectOpener.code-workspace contains required CMake settings:

- Generator: Visual Studio 18 2026
- Build directory: ${workspaceFolder}/build
- Qt prefix path: E:/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6
- Windows SDK tools:
  - CMAKE_MT=C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/mt.exe
  - CMAKE_RC_COMPILER=C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/rc.exe

If your SDK version differs, update those two paths to the installed version on your machine.

## First-time Configure Steps

1. Open the repository by opening ProjectOpener.code-workspace in VS Code.
2. Run CMake: Select a Kit and choose MSVC x64 (Visual Studio 2026).
3. Run CMake: Delete Cache and Reconfigure.
4. Run CMake: Build.

## Rehearsal From Clean State (Recommended)

If you need to reproduce setup from scratch (for example after installing Visual Studio or Qt), run this sequence from the repository root:

```powershell
if (Test-Path build-rehearsal) { Remove-Item -Recurse -Force build-rehearsal }
cmake -S . -B build-rehearsal -G "Visual Studio 18 2026" `
  -DCMAKE_PREFIX_PATH="E:/Qt/6.10.2/msvc2022_64/lib/cmake/Qt6" `
  -DCMAKE_MT="C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/mt.exe" `
  -DCMAKE_RC_COMPILER="C:/Program Files (x86)/Windows Kits/10/bin/10.0.26100.0/x64/rc.exe"
cmake --build build-rehearsal --config Release
```

Run the app:

```powershell
.\build-rehearsal\Release\ProjectOpener.exe
```

This command path is the fastest fallback when VS Code CMake Tools kit detection is inconsistent.

## Troubleshooting

- If configure fails with missing mt.exe or rc.exe, verify Windows SDK install and update paths in ProjectOpener.code-workspace.
- If Qt is not found, verify CMAKE_PREFIX_PATH points to your Qt6 package config folder.
- If settings do not seem to apply, reload the VS Code window and re-run Delete Cache and Reconfigure.
