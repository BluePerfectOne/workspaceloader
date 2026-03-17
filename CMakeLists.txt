cmake_minimum_required(VERSION 3.16)
project(ProjectOpener LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ProjectOpener WIN32
    src/main.cpp
)

# Pre-build: close ProjectOpener.exe if running
add_custom_command(
    TARGET ProjectOpener
    PRE_BUILD
    COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/close_ProjectOpener.ps1"
    COMMENT "Closing ProjectOpener.exe if running before build"
)

# Post-build: launch ProjectOpener.exe from Release folder
add_custom_command(
    TARGET ProjectOpener
    POST_BUILD
    COMMAND powershell -ExecutionPolicy Bypass -Command "Start-Process -NoNewWindow -FilePath \"$<TARGET_FILE:ProjectOpener>\" -WorkingDirectory \"${CMAKE_SOURCE_DIR}\""
    COMMENT "Launching ProjectOpener.exe after build with correct working directory"
)

# Needed for ShellExecuteW on some toolchains.
target_link_libraries(ProjectOpener PRIVATE shell32)

target_include_directories(ProjectOpener PRIVATE src)