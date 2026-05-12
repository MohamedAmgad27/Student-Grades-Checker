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
void loadTestData(GradeManager* backend) {
    // Inject Students
    backend->addStudent(1045, "Ahmed Raza");
    backend->addStudent(1046, "Fatima Khan");
    backend->addStudent(1047, "Hassan Ali");
    backend->addStudent(1048, "Ayesha Noor");
    backend->addStudent(1049, "Usman Javed");

    // Inject Grades to create different cGPAs
    // Ahmed (High GPA)
    backend->addTermGPA(1045, Term::Fall, 3.8);
    backend->addTermGPA(1045, Term::Spring, 3.9);
    
    // Fatima (Medium GPA)
    backend->addTermGPA(1046, Term::Fall, 3.2);
    backend->addTermGPA(1046, Term::Spring, 3.5);

    // Hassan (Lower GPA)
    backend->addTermGPA(1047, Term::Fall, 2.5);
    backend->addTermGPA(1047, Term::Spring, 2.9);

    // Ayesha (Top Student!)
    backend->addTermGPA(1048, Term::Fall, 4.0);
    backend->addTermGPA(1048, Term::Spring, 3.9);

    // Usman
    backend->addTermGPA(1049, Term::Fall, 3.1);
    backend->addTermGPA(1049, Term::Spring, 3.0);
}

MainWindow::MainWindow(GradeManager* manager,
                       QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    backend(manager)
{
    ui->setupUi(this);

loadTestData(backend);


    // Light mode override for the main window

    this->setStyleSheet(R"(
        QWidget { background-color: white; }
        QFrame { background-color: white; border-radius: 12px; }
    )");

    // ==========================================
    // 0. STARTUP STATE & DATABASE INIT
    // ==========================================
    // Force the app to open on the Welcome Screen
    ui->mainStack->setCurrentIndex(0);

    // Initialize the main backend database

    // Load the mock data into the tree immediately



    // ==========================================
    // 1. ROLE SELECTION (The Welcome Screen)
    // ==========================================
    
    // INSTRUCTOR LOGIN
    connect(ui->btn_instructor, &QPushButton::clicked, this, [=](){
        // Open the Instructor Dashboard AND pass the backend to it!
        Instructor *ins = new Instructor(backend,this);
        ins->show();
        this->hide();
        
        // Optional: you can hide the main window here if you want using this->hide();
    });

    connect(ui->btn_student, &QPushButton::clicked, this, [this]() {

        StudentLoginDialog* loginDialog =
            new StudentLoginDialog(backend, this);

        if (loginDialog->exec() == QDialog::Accepted) {

            Student* student =
                loginDialog->getLoggedStudent();

            StudentDashboard* dashboard =
                new StudentDashboard(student);


            dashboard->show();
            this->hide();

        }
    });


    // ==========================================
    // 2. LECTURER SIDEBAR NAVIGATION (Legacy/Alternative View)
    // ==========================================
    connect(ui->sidebarMenu, &QListWidget::currentRowChanged,
            ui->lecturerPanes, &QStackedWidget::setCurrentIndex);

    ui->sidebarMenu->setCurrentRow(0); // Default to first tab


    // ==========================================
    // 3. LECTURER DASHBOARD BUTTON CONNECTIONS
    // ==========================================
    connect(ui->btnOpenAddCourse, &QPushButton::clicked, this, &MainWindow::handleAddCourse);
    connect(ui->btnOpenInputGrade, &QPushButton::clicked, this, &MainWindow::handleRecordGrade);

    // Assign Lecturer Pop-up
    connect(ui->btnOpenAssginLecturer, &QPushButton::clicked, this, [this]() {
        AssignLecturerDialog dialog(this);
        if (dialog.exec() == QDialog::Accepted) {
            QMessageBox::information(this, "Success", "Lecturer Assigned Successfully!");
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

void MainWindow::onLecturerTabChanged(int index) {
}

void MainWindow::handleAddCourse() {
    AddCourseDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Success", "Course Added to Hash Table!");
    }
}

void MainWindow::handleRecordGrade() {
    InputGradeDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QMessageBox::information(this, "Success", "Student Grade Recorded!");
    }
}