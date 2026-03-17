#include <QApplication>
#include "ProjectLauncherWidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    ProjectLauncherWidget launcher;
    launcher.show();
    return app.exec();
}