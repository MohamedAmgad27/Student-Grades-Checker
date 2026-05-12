#include "instructor.h"
#include "ui_instructor.h"
#include "addcoursedialog.h"
#include "../src/GradeManager.hpp"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <algorithm> // Needed for std::sort

// Update constructor to accept the backend pointer
Instructor::Instructor(GradeManager* backendPtr, QWidget *parent)
    : QWidget(parent)

    , ui(new Ui::Instructor) ,backend(backendPtr)// <-- Save the database reference

{
    ui->setupUi(this);

    // initally open student page
    ui->stackedWidget->setCurrentWidget(ui->Studentpage);

    // Nav bar entrance

    connect(ui->btn_studentP, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->Studentpage);
        setWindowTitle("Students Dashboard");
    });
    connect(ui->btn_lecturerP, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->LecturerPage);
        setWindowTitle("Lecturers Dashboard");
    });
    connect(ui->btn_coursesP, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->CoursesPage);
        setWindowTitle("Courses Dashboard");
    });
    connect(ui->btn_reportsP, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentWidget(ui->ReportsPage);
        setWindowTitle("Reports Dashboard");
           loadTopStudentsReport();
    });

    // Report side bars
    connect(ui->btn_top, &QPushButton::clicked, this, [this]() {
        ui->reportstack->setCurrentWidget(ui->topstudentP);
        loadTopStudentsReport();

    });
    connect(ui->btn_pass, &QPushButton::clicked, this, [this]() {
        ui->reportstack->setCurrentWidget(ui->passfaillp);
        loadPassFailReport();
    });
    connect(ui->btn_grade, &QPushButton::clicked, this, [this]() {
        ui->reportstack->setCurrentWidget(ui->gradedis);
        loadGradeDistribution();
    });

    // Connect Buttons
    connect(ui->btn_addCourse, &QPushButton::clicked, this, &Instructor::on_btn_addCourse_clicked);
    connect(ui->btn_addLecturer, &QPushButton::clicked, this, &Instructor::on_btn_addLecturer_clicked);
    connect(ui->btn_addStudent, &QPushButton::clicked, this, &Instructor::on_btn_addStudent_clicked);

    // Run Visual Setups
    setupCoursesTable();
    setupLecturersTable();

    //// styles for histogram /////
    QString barStyle =
        "QProgressBar {"
        "border:1px solid #d1d5db;"
        "border-radius:8px;"
        "background:#e5e7eb;"
        "text-align:center;"
        "}"
        "QProgressBar::chunk {"
        "background:#2563eb;"
        "border-radius:8px;"
        "}";

    ui->barA->setStyleSheet(barStyle);
    ui->barB->setStyleSheet(barStyle);
    ui->barC->setStyleSheet(barStyle);
    ui->barD->setStyleSheet(barStyle);
    ui->barF->setStyleSheet(barStyle);

    setupStudentsTable();

}

Instructor::~Instructor()
{
    delete ui;
}



/////////Ui functions
void Instructor::setupCoursesTable()
{
    // FORCE LIGHT MODE COLORS FOR COURSES TAB
    ui->lineEdit_Search->setStyleSheet(
        "QLineEdit {"
        "  color: #111827;" 
        "  background-color: #f9fafb;"
        "  border: 1px solid #e5e7eb;"
        "  border-radius: 5px;"
        "  padding: 8px;"
        "}"
    );


    ui->coursesTable->setStyleSheet(
        "QTableWidget { background-color: white; color: #374151; gridline-color: #f3f4f6; border: none; }"
        "QTableWidget::viewport { background-color: white; }"
        "QHeaderView::section { background-color: white; color: #111827; font-weight: bold; border: none; border-bottom: 2px solid #e5e7eb; }"
        "QTableWidget::item { background-color: white; color: #111827; }"
        "QTableWidget::item:selected { background-color: #eff6ff; color: #1d4ed8; }"
    );


    // Table settings

    ui->coursesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->coursesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Dummy data
    ui->coursesTable->setRowCount(1);
    ui->coursesTable->setItem(0, 0, new QTableWidgetItem("CSE333"));
    ui->coursesTable->setItem(0, 1, new QTableWidgetItem("Data Structures"));
    ui->coursesTable->setItem(0, 2, new QTableWidgetItem("3"));
    ui->coursesTable->setItem(0, 3, new QTableWidgetItem("Unassigned"));
    
    ui->label_TableTitle->setText("All Courses (Total: 1)");
    ui->label_Showing->setText("Showing 1 to 1 of 1 courses");
}


void Instructor::setupLecturersTable()
{
    // Stretch columns to fill space
    ui->lecturersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lecturersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // Add dummy data row
    ui->lecturersTable->setRowCount(1);
    ui->lecturersTable->setItem(0, 0, new QTableWidgetItem("101"));
    ui->lecturersTable->setItem(0, 1, new QTableWidgetItem("Dr. Ahmed Raza"));
    ui->lecturersTable->setItem(0, 2, new QTableWidgetItem("Computer Science"));
    
    // Update labels
    ui->label_LecTableTitle->setText("All Lecturers (Total: 1)");
    ui->label_ShowingLec->setText("Showing 1 to 1 of 1 lecturers");
}


void Instructor::setupStudentsTable()
{
    // 1. Force Light Mode Colors
    ui->lineEdit_SearchStu->setStyleSheet(
        "QLineEdit { color: #111827; background-color: #f9fafb; border: 1px solid #e5e7eb; border-radius: 5px; padding: 8px; }"
    );
    ui->studentsTable->setStyleSheet(
        "QTableWidget { background-color: white; color: #374151; gridline-color: #f3f4f6; border: none; }"
        "QTableWidget::viewport { background-color: white; }"
        "QHeaderView::section { background-color: white; color: #111827; font-weight: bold; border: none; border-bottom: 2px solid #e5e7eb; }"
        "QTableWidget::item { background-color: white; color: #111827; }"
        "QTableWidget::item:selected { background-color: #eff6ff; color: #1d4ed8; }"
    );

    // 2. Format columns
    ui->studentsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->studentsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 3. DYNAMIC DATA INTEGRATION
    std::vector<Student*> allStudents = backend->getAllStudentsAsList();
    ui->studentsTable->setRowCount(allStudents.size());
    
    for (size_t i = 0; i < allStudents.size(); ++i) {
        Student* s = allStudents[i];
        QString gpaStr = QString::number(s->calculateCumulativeGPA(), 'f', 2);
        
        ui->studentsTable->setItem(i, 0, new QTableWidgetItem(QString::number(s->getID())));
        ui->studentsTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s->getName())));
        ui->studentsTable->setItem(i, 2, new QTableWidgetItem("General")); // Placeholder for Dept
        ui->studentsTable->setItem(i, 3, new QTableWidgetItem(gpaStr));
        ui->studentsTable->setItem(i, 4, new QTableWidgetItem("Active"));
    }
}




/////////button_clicked event
void Instructor::on_btn_addCourse_clicked()
{
    AddCourseDialog dialog(this);
    dialog.exec(); 
}

void Instructor::on_btn_addLecturer_clicked()
{
    QMessageBox::information(this, "Coming Soon", "The Add Lecturer dialog will open here!");
}

void Instructor::on_btn_addStudent_clicked()
{
    QMessageBox::information(this, "Coming Soon", "The Add Student dialog will open here!");

}




void Instructor::loadTopStudentsReport() {
    // prevent crash if it is not set
    if (!backend) {
        qWarning("GradeManager not set!");
        return;
    }

    // Clear existing table
    ui->topStudentsTable->setRowCount(0);

    // Get top 10 students by CGPA
    std::vector<Student*> topStudents = backend->getTopStudentsByCGPA(10);

    // Set up table
    ui->topStudentsTable->setColumnCount(4);
    ui->topStudentsTable->setHorizontalHeaderLabels(
        {"Rank", "Student ID", "Name", "CGPA"}
        );

    // Populate table
    ui->topStudentsTable->setRowCount(topStudents.size());

    for (int i = 0; i < topStudents.size(); i++) {
        Student* student = topStudents[i];

        // Rank
        ui->topStudentsTable->setItem(i, 0,
                                      new QTableWidgetItem(QString::number(i + 1))); ///(row,column,value)

        // Student ID
        ui->topStudentsTable->setItem(i, 1,
                                      new QTableWidgetItem(QString::number(student->getID())));

        // Name
        ui->topStudentsTable->setItem(i, 2,
                                      new QTableWidgetItem(QString::fromStdString(student->getName())));

        // CGPA
        ui->topStudentsTable->setItem(i, 4,
                                      new QTableWidgetItem(QString::number(student->calculateCumulativeGPA(), 'f', 2)));
    }


}void Instructor::loadPassFailReport() {
    if (!backend) {
        qWarning("GradeManager not set!");
        return;
    }

    int passCount = 0, failCount = 0;

    backend->getPassFailReport(passCount, failCount, 2.0); ////minimun gpa to pass is 2.0

    int total = passCount + failCount;

  ///// change text into the counts values
    ui->valueLabel_pass->setText(QString::number(passCount));
    ui->valueLabel_fail->setText(QString::number(failCount));
    ui->valueLabel_->setText(QString::number(total));



    // TABLE
    ui->passFailTable->setRowCount(0);

    std::vector<Student*> students =
        backend->getAllStudentsAsList();

    ////loop for each student see if its gpa indicates he fail or pass
    for(int i = 0; i < students.size(); i++) {

        Student* s = students[i];

        double gpa = s->calculateCumulativeGPA();

        QString status = (gpa >= 2.0) ? "PASS" : "FAIL";

        int row = ui->passFailTable->rowCount();
        ui->passFailTable->insertRow(row);

        ui->passFailTable->setItem(row, 0,
                                   new QTableWidgetItem(QString::number(s->getID())));

        ui->passFailTable->setItem(row, 1,
                                   new QTableWidgetItem(QString::fromStdString(s->getName())));

        ui->passFailTable->setItem(row, 2,
                                   new QTableWidgetItem(QString::number(gpa, 'f', 2)));

        ui->passFailTable->setItem(row, 3,
                                   new QTableWidgetItem(status));
    }
}

/////////////for distribution histogram

void Instructor::loadGradeDistribution() {

    if (!backend) {
        qWarning("GradeManager not set!");
        return;
    }

    std::map<std::string, int> distribution =
        backend->getGradeDistribution();

    int a = distribution["A"];
    int b = distribution["B"];
    int c = distribution["C"];
    int d = distribution["D"];
    int f = distribution["F"];

    ui->barA->setValue(a);
    ui->barB->setValue(b);
    ui->barC->setValue(c);
    ui->barD->setValue(d);
    ui->barF->setValue(f);

    ui->labelA->setText(QString("A\n%1").arg(a));
    ui->labelB->setText(QString("B\n%1").arg(b));
    ui->labelC->setText(QString("C\n%1").arg(c));
    ui->labelD->setText(QString("D\n%1").arg(d));
    ui->labelF->setText(QString("F\n%1").arg(f));
}
