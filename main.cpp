#include <QApplication>
#include "gui/mainwindow.h"
#include "src/GradeManager.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create the empty database
    GradeManager manager;

    // Launch the window
    MainWindow w(&manager);
    w.show();

    return a.exec();
}