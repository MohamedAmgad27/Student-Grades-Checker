#include <QApplication>
#include "gui/mainwindow.h"

#include "gui/instructor.h"
#include "src/GradeManager.hpp"
#include "src/Student.hpp"
#include "src/Lecturer.hpp"
#include "src/Course.hpp"

#include "src/Types.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    GradeManager manager;

    // ==========================================
    // Add Students
    // ==========================================

    manager.addStudent(1001, "Ahmed");
    manager.addStudent(1002, "Mariam");
    manager.addStudent(1003, "Omar");
    manager.addStudent(1004, "Sara");
    manager.addStudent(1005, "Youssef");

    // ==========================================
    // Add GPA Records
    // ==========================================

    manager.addTermGPA(1001, Term::Fall, 3.8);
    manager.addTermGPA(1001, Term::Spring, 3.9);

    manager.addTermGPA(1002, Term::Fall, 3.0);
    manager.addTermGPA(1002, Term::Spring, 3.2);

    manager.addTermGPA(1003, Term::Fall, 1.5);
    manager.addTermGPA(1003, Term::Spring, 1.8);

    manager.addTermGPA(1004, Term::Fall, 2.5);
    manager.addTermGPA(1004, Term::Spring, 2.7);

    manager.addTermGPA(1005, Term::Fall, 4.0);
    manager.addTermGPA(1005, Term::Spring, 3.9);


    MainWindow w(&manager);
    w.show();


    return a.exec();
}