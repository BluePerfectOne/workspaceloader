cmake_minimum_required(VERSION 3.16)
project(ProjectOpener VERSION 0.4.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

if (MSVC)
    add_compile_options(/Zc:__cplusplus /permissive-)
endif()

add_executable(ProjectOpener WIN32
    src/main.cpp
    src/ProjectLauncherWidget.cpp
    src/ProjectLauncherWidget.h
)

target_include_directories(ProjectOpener PRIVATE src)
target_compile_definitions(ProjectOpener PRIVATE PROJECT_OPENER_VERSION="${PROJECT_VERSION}")
target_link_libraries(ProjectOpener PRIVATE Qt6::Widgets)

add_custom_command(
    TARGET ProjectOpener
    PRE_BUILD
    COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/close_ProjectOpener.ps1"
    COMMENT "Closing ProjectOpener.exe if running before build"
)

get_target_property(QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
get_filename_component(QT_BIN_DIR "${QMAKE_EXECUTABLE}" DIRECTORY)
find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}")

if (WINDEPLOYQT_EXECUTABLE)
    add_custom_command(
        TARGET ProjectOpener
        POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-translations
                --release
                "$<TARGET_FILE:ProjectOpener>"
        COMMENT "Deploying Qt runtime files with windeployqt"
    )
else()
    message(WARNING "windeployqt not found - Qt runtime files will not be deployed automatically.")
endif()

add_custom_command(
    TARGET ProjectOpener
    POST_BUILD
    COMMAND powershell -ExecutionPolicy Bypass -Command "Start-Process -FilePath '$<TARGET_FILE:ProjectOpener>' -WorkingDirectory '${CMAKE_SOURCE_DIR}'"
    COMMENT "Launching ProjectOpener.exe after build"
)