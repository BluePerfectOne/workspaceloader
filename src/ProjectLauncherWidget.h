#pragma once
#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <vector>
#include <optional>
#include <string>

struct Project {
    std::string name;
    std::string path;
};

struct ProjectResolved {
    Project project;
    std::optional<std::string> workspace;
    bool pathExists = false;
};

class ProjectLauncherWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProjectLauncherWidget(QWidget* parent = nullptr);

private slots:
    void openSelectedProject();
    void openSelectedInExplorer();
    void showAboutDialog();
    void refreshProjects();
    void quitApp();
    void handleTopmostCheck(int state);
    void handleListDoubleClick(QListWidgetItem* item);
    void updateStatusLabel(int row);

private:
    void populateProjects();
    std::optional<std::string> findWorkspace(const std::string& dir);
    void launchTarget(const std::string& target);
    void launchExplorer(const std::string& path);
    void openChangelog();
    void openWorkingDirectory();
    std::string fileNameOnly(const std::string& fullPath);

    QListWidget* listWidget;
    QLabel* statusLabel;
    QPushButton* aboutButton;
    QPushButton* openButton;
    QPushButton* explorerButton;
    QPushButton* quitButton;
    QPushButton* refreshButton;
    QCheckBox* topmostCheck;
    std::vector<ProjectResolved> projects;
};
