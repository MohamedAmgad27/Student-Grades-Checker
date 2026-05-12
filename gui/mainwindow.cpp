#include "mainwindow.h"
#include "ui_mainwindow.h"

// Include the headers for your pop-up windows
#include "addcoursedialog.h"
#include "assignlecturerdialog.h"
#include "inputgradedialog.h"
#include "instructor.h"
#include "studentlogindialog.h"
#include "../src/GradeManager.hpp"
#include "studentdashboard.h"
#include <QMessageBox>
#include <QPushButton>

// helper function to load data
// helper function to load data
void loadTestData(GradeManager* backend) {
    // ==========================================
    // 1. Inject 5 Lecturers
    // ==========================================
    backend->addLecturer(new Lecturer(101, "Dr. Ahmed Raza", "Computer Science"));
    backend->addLecturer(new Lecturer(102, "Dr. Sarah Johnson", "Software Engineering"));
    backend->addLecturer(new Lecturer(103, "Dr. Tarek Amr", "Information Technology"));
    backend->addLecturer(new Lecturer(104, "Dr. Laila Mahmoud", "Artificial Intelligence"));
    backend->addLecturer(new Lecturer(105, "Dr. Karim Fathy", "Mathematics"));

    // ==========================================
    // 2. Inject 5 Courses
    // ==========================================
    backend->addCourse(new Course(3, "CSE333", "Data Structures", backend->searchLecturer(101)));
    backend->addCourse(new Course(4, "CSE444", "Algorithms", backend->searchLecturer(102)));
    backend->addCourse(new Course(3, "SWE222", "Software Design", backend->searchLecturer(103)));
    backend->addCourse(new Course(4, "MTH101", "Calculus I", backend->searchLecturer(105)));
    
    // Deliberately leaving AI500 unassigned to test the "Unassigned" UI logic
    backend->addCourse(new Course(3, "AI500", "Machine Learning", nullptr));

    // ==========================================
    // 3. Inject 5 Students
    // ==========================================
    backend->addStudent(1045, "Ahmed Raza");
    backend->addStudent(1046, "Fatima Khan");
    backend->addStudent(1047, "Hassan Ali");
    backend->addStudent(1048, "Ayesha Noor");
    backend->addStudent(1049, "Usman Javed");

    // ==========================================
    // 4. Register Courses & Assign Grades 
    // ==========================================

    // 1. Ahmed - High Achiever (Solid Pass)
    backend->registerStudentForCourse(1045, "CSE333", Term::Fall, 3);
    backend->gradeStudentCourse(1045, "CSE333", 3.8); 
    backend->registerStudentForCourse(1045, "CSE444", Term::Spring, 4);
    backend->gradeStudentCourse(1045, "CSE444", 3.9);
    backend->registerStudentForCourse(1045, "MTH101", Term::Fall, 4);
    backend->gradeStudentCourse(1045, "MTH101", 4.0);
    backend->registerStudentForCourse(1045, "AI500", Term::Spring, 3);
    backend->gradeStudentCourse(1045, "AI500", -1.0); // In Progress

    // 2. Fatima - Good Student (Pass)
    backend->registerStudentForCourse(1046, "CSE333", Term::Fall, 3);
    backend->gradeStudentCourse(1046, "CSE333", 3.2);
    backend->registerStudentForCourse(1046, "SWE222", Term::Spring, 3);
    backend->gradeStudentCourse(1046, "SWE222", 3.5);
    backend->registerStudentForCourse(1046, "MTH101", Term::Fall, 4);
    backend->gradeStudentCourse(1046, "MTH101", 2.8);

    // 3. Hassan - Struggling Student (Fail - CGPA under 2.0)
    backend->registerStudentForCourse(1047, "CSE333", Term::Fall, 3);
    backend->gradeStudentCourse(1047, "CSE333", 1.5); 
    backend->registerStudentForCourse(1047, "MTH101", Term::Fall, 4);
    backend->gradeStudentCourse(1047, "MTH101", 1.0); 
    backend->registerStudentForCourse(1047, "SWE222", Term::Spring, 3);
    backend->gradeStudentCourse(1047, "SWE222", -1.0); // In Progress

    // 4. Ayesha - The Valedictorian (Perfect 4.0)
    backend->registerStudentForCourse(1048, "CSE444", Term::Fall, 4);
    backend->gradeStudentCourse(1048, "CSE444", 4.0);
    backend->registerStudentForCourse(1048, "SWE222", Term::Spring, 3);
    backend->gradeStudentCourse(1048, "SWE222", 4.0);
    backend->registerStudentForCourse(1048, "AI500", Term::Spring, 3);
    backend->gradeStudentCourse(1048, "AI500", 4.0);

    // 5. Usman - The Borderline Student (Barely Passing)
    backend->registerStudentForCourse(1049, "SWE222", Term::Fall, 3);
    backend->gradeStudentCourse(1049, "SWE222", 2.1);
    backend->registerStudentForCourse(1049, "CSE333", Term::Fall, 3);
    backend->gradeStudentCourse(1049, "CSE333", 1.8);
    backend->registerStudentForCourse(1049, "MTH101", Term::Spring, 4);
    backend->gradeStudentCourse(1049, "MTH101", 2.0);
    backend->registerStudentForCourse(1049, "CSE444", Term::Spring, 4);
    backend->gradeStudentCourse(1049, "CSE444", -1.0); // In Progress
}

MainWindow::MainWindow(GradeManager* manager, QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    backend(manager) // <-- This saves the pointer to the class variable 'backend'
{
    ui->setupUi(this);

    // Light mode override for the main window
    this->setStyleSheet(R"(
        QWidget { background-color: white; }
        QFrame { background-color: white; border-radius: 12px; }
    )");

    // ==========================================
    // 0. STARTUP STATE & DATABASE INIT
    // ==========================================
    ui->mainStack->setCurrentIndex(0);

    // If main.cpp didn't give us a database, create one!
    if (backend == nullptr) {
        backend = new GradeManager();
    }

    // FIXED: Actually call the function to load the mock data!
    loadTestData(backend);


    // ==========================================
    // 1. ROLE SELECTION (The Welcome Screen)
    // ==========================================

    // INSTRUCTOR LOGIN
    connect(ui->btn_instructor, &QPushButton::clicked, this, [=](){

        // FIXED: Pass 'nullptr' so it opens as an independent window!
        Instructor *ins = new Instructor(backend, nullptr);
        ins->setAttribute(Qt::WA_DeleteOnClose); // Cleans up memory when closed
        ins->show();

        this->hide();
    });

    // STUDENT LOGIN
    connect(ui->btn_student, &QPushButton::clicked, this, [=]() {

        // Keep 'this' for the dialog so it stays centered on the welcome screen
        StudentLoginDialog* loginDialog = new StudentLoginDialog(backend, this);

        if (loginDialog->exec() == QDialog::Accepted) {
            Student* student = loginDialog->getLoggedStudent();

            // FIXED: Pass 'nullptr' to the dashboard so it opens independently!
            StudentDashboard* dashboard = new StudentDashboard(student, nullptr);
            dashboard->setAttribute(Qt::WA_DeleteOnClose);
            dashboard->show();

            this->hide();
        }
        delete loginDialog;
    });

    // ==========================================
    // 2. LECTURER SIDEBAR NAVIGATION (Legacy View)
    // ==========================================
    connect(ui->sidebarMenu, &QListWidget::currentRowChanged,
            ui->lecturerPanes, &QStackedWidget::setCurrentIndex);

    ui->sidebarMenu->setCurrentRow(0); // Default to first tab

    // ==========================================
    // 3. LECTURER DASHBOARD BUTTON CONNECTIONS
    // ==========================================

    // FIXED: Pass 'backend' into your handlers
    connect(ui->btnOpenAddCourse, &QPushButton::clicked, this, [=]() {
        handleAddCourse(backend);
    });

    connect(ui->btnOpenInputGrade, &QPushButton::clicked, this, [=]() {
        handleRecordGrade(backend);
    });

    connect(ui->btnOpenAssginLecturer, &QPushButton::clicked, this, [=]() {
        AssignLecturerDialog dialog(backend, this);
        if (dialog.exec() == QDialog::Accepted) {
            // Success handled internally
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ==========================================
// 4. THE SLOT IMPLEMENTATIONS
// ==========================================

void MainWindow::onLecturerTabChanged(int index)
{
}

// Notice we updated this to receive the GradeManager pointer!
void MainWindow::handleAddCourse(GradeManager* backend) {
    // FIXED: Pass 'backend' as the first argument!
    AddCourseDialog dialog(backend, this);

    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Success", "Course Added to Hash Table!");
    }
}

void MainWindow::handleRecordGrade(GradeManager* backend) {
    // Note: If you haven't updated InputGradeDialog's constructor yet,
    // leave this as dialog(this) until we fix that file, otherwise it will throw the same error!

    InputGradeDialog dialog(this); // (We will change this to (backend, this) soon!)
    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Success", "Student Grade Recorded!");
    }
}