#include "instructor.h"
#include "ui_instructor.h"
#include "addcoursedialog.h"
#include "assignlecturerdialog.h"
#include "../src/GradeManager.hpp"
// Include the headers so the UI knows how to read Course and Lecturer objects
#include "../src/Course.hpp"
#include "../src/Lecturer.hpp"

// --- ADDED: Includes for your new student dialogs ---
#include "addstudentdialog.h"
#include "searchstudentdialog.h"
#include "updatestudentdialog.h"
#include "deletestudentdialog.h"

#include <QTableWidgetItem>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <algorithm> // Needed for std::sort

// Update constructor to accept the backend pointer
Instructor::Instructor(GradeManager* backendPtr, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Instructor) 
    , backend(backendPtr) // <-- Save the database reference
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

    connect(ui->lineEdit_SearchStu, &QLineEdit::textChanged, this, &Instructor::filterStudentsTable);

    // Run Visual Setups
    setupCoursesTable();
    setupLecturersTable();
    setupStudentsTable();

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

    // DYNAMIC DATA INTEGRATION (Requires backend->getAllCourses() to be implemented)
    if (!backend) return;
    
    std::vector<Course*> allCourses = backend->getAllCourses();
    ui->coursesTable->setRowCount(allCourses.size());
    
    for (size_t i = 0; i < allCourses.size(); ++i) {
        Course* c = allCourses[i];
        
        ui->coursesTable->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c->getCourseCode())));
        ui->coursesTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c->getCourseName())));
        ui->coursesTable->setItem(i, 2, new QTableWidgetItem(QString::number(c->getCreditHours())));
        
        // Handle unassigned lecturers gracefully
        QString lecName = (c->getAssignedLecturer() != nullptr) 
                          ? QString::fromStdString(c->getAssignedLecturer()->getName()) 
                          : "Unassigned";
        ui->coursesTable->setItem(i, 3, new QTableWidgetItem(lecName));
    }
    
    ui->label_TableTitle->setText(QString("All Courses (Total: %1)").arg(allCourses.size()));
}


void Instructor::setupLecturersTable()
{
    // Stretch columns to fill space
    ui->lecturersTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->lecturersTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // DYNAMIC DATA INTEGRATION (Requires backend->getAllLecturers() to be implemented)
    if (!backend) return;

    std::vector<Lecturer*> allLecturers = backend->getAllLecturers();
    ui->lecturersTable->setRowCount(allLecturers.size());

    for (size_t i = 0; i < allLecturers.size(); ++i) {
        Lecturer* l = allLecturers[i];
        
        ui->lecturersTable->setItem(i, 0, new QTableWidgetItem(QString::number(l->getID())));
        ui->lecturersTable->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(l->getName())));
        ui->lecturersTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(l->getDepartment())));
    }
    
    ui->label_LecTableTitle->setText(QString("All Lecturers (Total: %1)").arg(allLecturers.size()));
}

void Instructor::on_btn_deleteCourse_clicked()
{
    // 1. Figure out which row the user clicked on
    int currentRow = ui->coursesTable->currentRow();

    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please click on a course in the table first to select it.");
        return;
    }

    // 2. Grab the Course Code from Column 0
    QString courseCode = ui->coursesTable->item(currentRow, 0)->text();

    // 3. Ask for confirmation
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete",
                                                              "Are you sure you want to completely delete " + courseCode + "?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 4. Delete it from the backend and refresh everything!
        if (backend->deleteCourse(courseCode.toStdString())) {

            setupCoursesTable(); // Redraw the courses table

            // Deleting a course affects student GPAs! Refresh the rest of the UI:
            setupStudentsTable();
            loadTopStudentsReport();
            loadPassFailReport();
            loadGradeDistribution();

            QMessageBox::information(this, "Success", "Course " + courseCode + " deleted.");
        }
    }
}

void Instructor::on_btn_updateCourse_clicked()
{
    int currentRow = ui->coursesTable->currentRow();

    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please click on a course in the table first to update it.");
        return;
    }

    QString courseCode = ui->coursesTable->item(currentRow, 0)->text();
    Course* course = backend->searchCourse(courseCode.toStdString());

    if (course != nullptr) {
        bool ok;

        // Ask for the new Course Name
        QString newName = QInputDialog::getText(this, "Update Course", "New Course Name:",
                                                QLineEdit::Normal, QString::fromStdString(course->getCourseName()), &ok);

        if (ok && !newName.isEmpty()) {
            course->setCourseName(newName.toStdString());

            // Ask for the new Credit Hours
            int newHours = QInputDialog::getInt(this, "Update Course", "New Credit Hours:",
                                                course->getCreditHours(), 1, 10, 1, &ok);
            if (ok) {
                course->setCreditHours(newHours);

                setupCoursesTable(); // Redraw the table to show the new values!

                // CRITICAL: Changing credit hours changes CGPA calculations! Refresh UI:
                setupStudentsTable();
                loadTopStudentsReport();
                loadPassFailReport();
                loadGradeDistribution();

                QMessageBox::information(this, "Success", "Course updated successfully!");
            }
        }
    }
}

void Instructor::on_lineEdit_Search_textChanged(const QString &text)
{
    // Loop through every row in the Courses table
    for (int i = 0; i < ui->coursesTable->rowCount(); ++i) {
        bool match = false;
        
        // Check all columns in that row to see if the text matches
        for (int j = 0; j < ui->coursesTable->columnCount(); ++j) {
            QTableWidgetItem *item = ui->coursesTable->item(i, j);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        
        // Hide the row if it doesn't match the search text
        ui->coursesTable->setRowHidden(i, !match);
    }
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
    if (!backend) return;

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
    // Pass the backend into the dialog!
    AddCourseDialog dialog(backend, this);

    // If the user clicks Save and it succeeds, redraw the table
    if (dialog.exec() == QDialog::Accepted) {
        setupCoursesTable();

        // Refresh reports just to be safe
        setupStudentsTable();
        loadTopStudentsReport();
        loadPassFailReport();
        loadGradeDistribution();
    }
}

void Instructor::on_btn_assignLecturer_clicked()
{
    // 1. Pass the backend to the dialog
    AssignLecturerDialog dialog(backend, this);

    // 2. If the user successfully assigns a lecturer, instantly refresh the Courses table!
    if (dialog.exec() == QDialog::Accepted) {
        setupCoursesTable();
    }
}

// ==========================================
// LECTURERS TAB LOGIC
// ==========================================

void Instructor::on_lineEdit_SearchLec_textChanged(const QString &text)
{
    // Live Search for Lecturers Table
    for (int i = 0; i < ui->lecturersTable->rowCount(); ++i) {
        bool match = false;
        for (int j = 0; j < ui->lecturersTable->columnCount(); ++j) {
            QTableWidgetItem *item = ui->lecturersTable->item(i, j);
            if (item && item->text().contains(text, Qt::CaseInsensitive)) {
                match = true;
                break;
            }
        }
        ui->lecturersTable->setRowHidden(i, !match);
    }
}

void Instructor::on_btn_addLecturer_clicked()
{
    bool ok;

    // 1. Ask for Lecturer ID
    int id = QInputDialog::getInt(this, "Add Lecturer", "Enter Lecturer ID:", 1, 1, 99999, 1, &ok);
    if (!ok) return; // User clicked Cancel

    // 2. Check for duplicates early
    if (backend->searchLecturer(id) != nullptr) {
        QMessageBox::warning(this, "Duplicate Error", "A Lecturer with this ID already exists!");
        return;
    }

    // 3. Ask for Lecturer Name
    QString name = QInputDialog::getText(this, "Add Lecturer", "Enter Lecturer Name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    // 4. Ask for Department
    QString dept = QInputDialog::getText(this, "Add Lecturer", "Enter Department:", QLineEdit::Normal, "", &ok);
    if (!ok || dept.trimmed().isEmpty()) return;

    // 5. Create the Lecturer and add to backend
    Lecturer* newLecturer = new Lecturer(id, name.toStdString(), dept.toStdString());
    backend->addLecturer(newLecturer);

    // 6. Notify and Refresh Table!
    QMessageBox::information(this, "Success", "Lecturer added successfully!");
    setupLecturersTable();
}

void Instructor::on_btn_deleteLecturer_clicked()
{
    int currentRow = ui->lecturersTable->currentRow();
    
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please click on a Lecturer in the table first to select them.");
        return;
    }

    // Grab the ID from Column 0 and Name from Column 1
    int lecId = ui->lecturersTable->item(currentRow, 0)->text().toInt();
    QString lecName = ui->lecturersTable->item(currentRow, 1)->text();

    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Delete", 
                                        "Are you sure you want to completely delete " + lecName + "?\n(They will be unassigned from any active courses).", 
                                        QMessageBox::Yes | QMessageBox::No);
                                        
    if (reply == QMessageBox::Yes) {
        if (backend->deleteLecturer(lecId)) {
            QMessageBox::information(this, "Success", "Lecturer deleted.");
            setupLecturersTable(); // Refresh Lecturers table
            setupCoursesTable();   // Refresh Courses table (in case they were removed from a course!)
        }
    }
}

void Instructor::on_btn_updateLecturer_clicked()
{
    int currentRow = ui->lecturersTable->currentRow();
    
    if (currentRow < 0) {
        QMessageBox::warning(this, "Error", "Please click on a Lecturer in the table first to update them.");
        return;
    }

    int lecId = ui->lecturersTable->item(currentRow, 0)->text().toInt();
    Lecturer* lec = backend->searchLecturer(lecId);
    
    if (lec != nullptr) {
        bool ok;
        
        // Ask for the new Name
        QString newName = QInputDialog::getText(this, "Update Lecturer", "New Name:", 
                                                QLineEdit::Normal, QString::fromStdString(lec->getName()), &ok);
                                                
        if (ok && !newName.isEmpty()) {
            lec->setName(newName.toStdString());
            
            // Ask for the new Department
            QString newDept = QInputDialog::getText(this, "Update Lecturer", "New Department:", 
                                                    QLineEdit::Normal, QString::fromStdString(lec->getDepartment()), &ok);
            if (ok && !newDept.isEmpty()) {
                lec->setDepartment(newDept.toStdString());
                
                QMessageBox::information(this, "Success", "Lecturer updated successfully!");
                setupLecturersTable(); // Redraw the table
            }
        }
    }
}

// --- UPDATED: Replaced the 'Coming Soon' message with the actual logic ---
void Instructor::on_btn_addStudent_clicked()
{
    AddStudentDialog dialog(backend, this);

    // ONLY refresh if the user actually saved the new student
    if (dialog.exec() == QDialog::Accepted) {

        // 1. Refresh the main Students tab
        setupStudentsTable();

        // 2. Refresh ALL the Report charts so the math stays accurate
        loadTopStudentsReport();
        loadPassFailReport();
        loadGradeDistribution();

        // Optional: Let the user know it worked
        QMessageBox::information(this, "Success", "Student added and dashboards updated!");
    }
}

// --- ADDED: The handlers for the new dialogs ---
void Instructor::on_btn_searchStudent_clicked()
{
    SearchStudentDialog dialog(backend, this);
    dialog.exec();
}

void Instructor::on_btn_updateStudent_clicked()
{
    UpdateStudentDialog dialog(backend, this);

    // ONLY refresh if the user actually saved the changes
    if (dialog.exec() == QDialog::Accepted) {

        // 1. Refresh the main Students tab
        setupStudentsTable();

        // 2. Refresh ALL the Report charts to keep the dashboard accurate
        loadTopStudentsReport();
        loadPassFailReport();
        loadGradeDistribution();

        // Optional: Let the user know it worked
        QMessageBox::information(this, "Success", "Student updated and dashboards refreshed!");
    }
}

void Instructor::on_btn_deleteStudent_clicked()
{
    DeleteStudentDialog dialog(backend, this);

    // ONLY refresh if the user actually confirmed the deletion
    if (dialog.exec() == QDialog::Accepted) {

        // 1. Refresh the main Students tab
        setupStudentsTable();

        // 2. Refresh ALL the Report charts so the math stays accurate
        loadTopStudentsReport();
        loadPassFailReport();
        loadGradeDistribution();

        // Optional: Let the user know it worked
        QMessageBox::information(this, "Success", "Student deleted and dashboards updated!");
    }
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

    for (size_t i = 0; i < topStudents.size(); i++) {
        Student* student = topStudents[i];

        // Rank
        ui->topStudentsTable->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1))); 

        // Student ID
        ui->topStudentsTable->setItem(i, 1, new QTableWidgetItem(QString::number(student->getID())));

        // Name
        ui->topStudentsTable->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(student->getName())));

        // CGPA (FIXED: Was 4, changed to 3 to match 4-column setup)
        ui->topStudentsTable->setItem(i, 3, new QTableWidgetItem(QString::number(student->calculateCumulativeGPA(), 'f', 2)));
    }
}

void Instructor::loadPassFailReport() {
    if (!backend) {
        qWarning("GradeManager not set!");
        return;
    }

    int passCount = 0, failCount = 0;

    backend->getPassFailReport(passCount, failCount, 1.0); ////minimun gpa to pass is 1.0

    int total = passCount + failCount;

    ///// change text into the counts values
    ui->valueLabel_pass->setText(QString::number(passCount));
    ui->valueLabel_fail->setText(QString::number(failCount));
    ui->valueLabel_->setText(QString::number(total));

    // TABLE
    ui->passFailTable->setRowCount(0);

    std::vector<Student*> students = backend->getAllStudentsAsList();

    ////loop for each student see if its gpa indicates he fail or pass
    for(size_t i = 0; i < students.size(); i++) {
        Student* s = students[i];
        double gpa = s->calculateCumulativeGPA();
        QString status = (gpa >= 1.0) ? "PASS" : "FAIL";

        int row = ui->passFailTable->rowCount();
        ui->passFailTable->insertRow(row);

        ui->passFailTable->setItem(row, 0, new QTableWidgetItem(QString::number(s->getID())));
        ui->passFailTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(s->getName())));
        ui->passFailTable->setItem(row, 2, new QTableWidgetItem(QString::number(gpa, 'f', 2)));
        ui->passFailTable->setItem(row, 3, new QTableWidgetItem(status));
    }
}

/////////////for distribution histogram
void Instructor::loadGradeDistribution() {

    if (!backend) {
        qWarning("GradeManager not set!");
        return;
    }

    std::map<std::string, int> distribution = backend->getGradeDistribution();

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

// ====================================================================
// LIVE SEARCH BAR LOGIC
// ====================================================================
void Instructor::filterStudentsTable(const QString &searchText)
{
    // Loop through every row currently in the table
    for (int i = 0; i < ui->studentsTable->rowCount(); ++i) {
        bool matchFound = false;

        // Grab the ID and Name cells for this specific row
        QTableWidgetItem *idItem = ui->studentsTable->item(i, 0);
        QTableWidgetItem *nameItem = ui->studentsTable->item(i, 1);

        // Check if the ID contains the typed text (Case Insensitive)
        if (idItem && idItem->text().contains(searchText, Qt::CaseInsensitive)) {
            matchFound = true;
        }

        // Check if the Name contains the typed text (Case Insensitive)
        if (nameItem && nameItem->text().contains(searchText, Qt::CaseInsensitive)) {
            matchFound = true;
        }

        // If a match was found, show the row (setRowHidden = false). Otherwise, hide it.
        ui->studentsTable->setRowHidden(i, !matchFound);
    }
}