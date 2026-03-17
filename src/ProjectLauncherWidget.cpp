#include "ProjectLauncherWidget.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
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
    setWindowTitle("Project Launcher");
    resize(590, 320);

    auto* mainLayout = new QVBoxLayout(this);
    auto* label = new QLabel("Select a project to open in VS Code:", this);
    mainLayout->addWidget(label);

    listWidget = new QListWidget(this);
    mainLayout->addWidget(listWidget);

    auto* buttonLayout = new QHBoxLayout();
    openButton = new QPushButton("Open", this);
    quitButton = new QPushButton("Quit", this);
    refreshButton = new QPushButton("Refresh", this);
    topmostCheck = new QCheckBox("Keep window on top", this);

    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(topmostCheck);
    buttonLayout->addStretch();
    buttonLayout->addWidget(openButton);
    buttonLayout->addWidget(quitButton);
    mainLayout->addLayout(buttonLayout);

    connect(openButton, &QPushButton::clicked, this, &ProjectLauncherWidget::openSelectedProject);
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

void ProjectLauncherWidget::refreshProjects() {
    populateProjects();
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
    openSelectedProject();
}
