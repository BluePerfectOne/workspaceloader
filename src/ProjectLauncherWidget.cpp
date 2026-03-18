#include "ProjectLauncherWidget.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QString>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QWidget>
#include <windows.h>
#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>

ProjectLauncherWidget::ProjectLauncherWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(QString("Project Launcher v%1").arg(PROJECT_OPENER_VERSION));
    resize(590, 320);

    auto* mainLayout = new QVBoxLayout(this);
    auto* label = new QLabel("Select a project to open in VS Code:", this);
    mainLayout->addWidget(label);

    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget);

    auto* buttonLayout = new QHBoxLayout();
    aboutButton = new QPushButton("About", this);
    openButton = new QPushButton("Open", this);
    explorerButton = new QPushButton("Explorer", this);
    quitButton = new QPushButton("Quit", this);
    refreshButton = new QPushButton("Refresh", this);
    topmostCheck = new QCheckBox("Keep window on top", this);

    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(topmostCheck);
    buttonLayout->addStretch();
    buttonLayout->addWidget(aboutButton);
    buttonLayout->addWidget(explorerButton);
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(quitButton);
    mainLayout->addLayout(buttonLayout);

    connect(aboutButton, &QPushButton::clicked, this, &ProjectLauncherWidget::showAboutDialog);
    connect(openButton, &QPushButton::clicked, this, &ProjectLauncherWidget::openSelectedProject);
    connect(explorerButton, &QPushButton::clicked, this, &ProjectLauncherWidget::openSelectedInExplorer);
    connect(quitButton, &QPushButton::clicked, this, &ProjectLauncherWidget::quitApp);
    connect(refreshButton, &QPushButton::clicked, this, &ProjectLauncherWidget::refreshProjects);
    connect(topmostCheck, &QCheckBox::stateChanged, this, &ProjectLauncherWidget::handleTopmostCheck);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &ProjectLauncherWidget::handleListDoubleClick);

    populateProjects();
}

void ProjectLauncherWidget::populateProjects() {
    projects.clear();
    listWidget->clear();

    std::ifstream f("projects.json");
    if (!f) {
        QMessageBox::critical(this, "Config Error", "projects.json file not found.");
        return;
    }
    try {
        nlohmann::json j;
        f >> j;
        for (const auto& item : j) {
            Project p;
            p.name = item["name"].get<std::string>();
            p.path = item["path"].get<std::string>();
            ProjectResolved resolved{p, findWorkspace(p.path)};
            std::string label = p.name;
            if (resolved.workspace.has_value()) {
                label += "  [" + fileNameOnly(*resolved.workspace) + "]";
            } else {
                label += "  [no workspace file found]";
            }
            listWidget->addItem(QString::fromStdString(label));
            projects.push_back(std::move(resolved));
        }
        if (!projects.empty()) {
            listWidget->setCurrentRow(0);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "JSON Parse Error", e.what());
    }
}

std::optional<std::string> ProjectLauncherWidget::findWorkspace(const std::string& dir) {
    try {
        if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
            return std::nullopt;
        }
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".code-workspace") {
                return entry.path().string();
            }
        }
    } catch (...) {
        return std::nullopt;
    }
    return std::nullopt;
}

std::string ProjectLauncherWidget::fileNameOnly(const std::string& fullPath) {
    return std::filesystem::path(fullPath).filename().string();
}

void ProjectLauncherWidget::launchTarget(const std::string& target) {
    QProcess* process = new QProcess(this);
    process->start("code", QStringList()
                           << "--reuse-window"
                           << QString::fromStdString(target));
    if (!process->waitForStarted()) {
        QMessageBox::critical(this, "Launch Error",
            "Failed to launch VS Code with command 'code'.\n"
            "Make sure VS Code is installed and the 'code' command is available in PATH.");
    }
}

void ProjectLauncherWidget::launchExplorer(const std::string& path) {
    const auto nativePath = QDir::toNativeSeparators(QString::fromStdString(path));
    if (!QProcess::startDetached("explorer.exe", QStringList() << nativePath)) {
        QMessageBox::critical(this, "Launch Error",
            "Failed to open File Explorer.\n"
            "Make sure explorer.exe is available.");
    }
}

void ProjectLauncherWidget::openChangelog() {
    const auto changelogPath = QDir::current().absoluteFilePath("CHANGELOG.md");
    if (!QFile::exists(changelogPath)) {
        QMessageBox::warning(this, "File Not Found", "CHANGELOG.md was not found in the working directory.");
        return;
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(changelogPath))) {
        QMessageBox::warning(this, "Open Failed", "Failed to open CHANGELOG.md.");
    }
}

void ProjectLauncherWidget::openWorkingDirectory() {
    launchExplorer(QDir::currentPath().toStdString());
}

void ProjectLauncherWidget::openSelectedProject() {
    int index = listWidget->currentRow();
    if (index < 0 || index >= static_cast<int>(projects.size())) {
        QMessageBox::warning(this, "No Selection", "Please select a project first.");
        return;
    }
    const auto& selected = projects[index];
    if (selected.workspace.has_value()) {
        launchTarget(*selected.workspace);
    } else {
        launchTarget(selected.project.path);
    }
}

void ProjectLauncherWidget::openSelectedInExplorer() {
    int index = listWidget->currentRow();
    if (index < 0 || index >= static_cast<int>(projects.size())) {
        QMessageBox::warning(this, "No Selection", "Please select a project first.");
        return;
    }

    const auto& selected = projects[index];
    if (!std::filesystem::exists(selected.project.path)) {
        QMessageBox::warning(this, "Path Not Found", "The selected project path does not exist.");
        return;
    }

    launchExplorer(selected.project.path);
}

void ProjectLauncherWidget::refreshProjects() {
    populateProjects();
}

void ProjectLauncherWidget::showAboutDialog() {
    const auto workingDirectory = QDir::toNativeSeparators(QDir::currentPath());
    const auto changelogPath = QDir::toNativeSeparators(QDir::current().absoluteFilePath("CHANGELOG.md"));
    const auto projectsJsonPath = QDir::toNativeSeparators(QDir::current().absoluteFilePath("projects.json"));
    const auto buildStamp = QString("%1 %2").arg(__DATE__).arg(__TIME__);

    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle("About Project Launcher");
    aboutBox.setIcon(QMessageBox::Information);
    aboutBox.setText(QString("Project Launcher v%1").arg(PROJECT_OPENER_VERSION));
    aboutBox.setInformativeText(
        "Quick launcher for opening projects in VS Code.\n\n"
        "Build: " + buildStamp + "\n\n"
        "projects.json: " + projectsJsonPath + "\n"
        "Changelog: " + changelogPath + "\n"
        "Workspace folder: " + workingDirectory + "\n\n"
        "Use 'Open Changelog' for release notes or 'Open Workspace Folder' to open this tool's folder.");

    auto* openChangelogButton = aboutBox.addButton("Open Changelog", QMessageBox::ActionRole);
    auto* openWorkspaceFolderButton = aboutBox.addButton("Open Workspace Folder", QMessageBox::ActionRole);
    aboutBox.addButton(QMessageBox::Ok);
    aboutBox.exec();

    if (aboutBox.clickedButton() == openChangelogButton) {
        openChangelog();
        return;
    }

    if (aboutBox.clickedButton() == openWorkspaceFolderButton) {
        openWorkingDirectory();
    }
}

void ProjectLauncherWidget::quitApp() {
    QApplication::quit();
}

#include <windows.h>

void ProjectLauncherWidget::handleTopmostCheck(int state)
{
    HWND hwnd = reinterpret_cast<HWND>(winId());
    SetWindowPos(hwnd,
                 state == Qt::Checked ? HWND_TOPMOST : HWND_NOTOPMOST,
                 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void ProjectLauncherWidget::handleListDoubleClick(QListWidgetItem* item) {
    Q_UNUSED(item);

    if (QApplication::keyboardModifiers().testFlag(Qt::ControlModifier)) {
        openSelectedInExplorer();
        return;
    }

    openSelectedProject();
}
