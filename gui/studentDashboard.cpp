#include "studentdashboard.h"
#include "ui_studentdashboard.h"
#include "mainwindow.h"
#include <QTableWidgetItem>
#include <QString>

StudentDashboard::StudentDashboard(Student* student, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StudentDashboard)
    , currentStudent(student)
{
    ui->setupUi(this);


    loadStudentData();
}


StudentDashboard::~StudentDashboard()
{
    delete ui;
}

void StudentDashboard::loadStudentData()
{
    if(currentStudent == nullptr)
        return;

    // Basic info
    ui->labelName->setText(
        QString::fromStdString(currentStudent->getName()));

    ui->labelID->setText(
        QString::number(currentStudent->getID()));

    ui->labelCGPA->setText(
        QString::number(currentStudent->calculateCumulativeGPA(), 'f', 2));

    ui->labelHours->setText(
        QString::number(currentStudent->getTotalCreditHours()));

    // Academic history table
    auto history = currentStudent->getTermGPAs();

    ui->historyTable->setRowCount(history.size());

    for(int i = 0; i < history.size(); i++)
    {
        QString term;

        switch(history[i].first)
        {
        case Term::Fall:
            term = "Fall";
            break;

        case Term::Spring:
            term = "Spring";
            break;

        case Term::Summer:
            term = "Summer";
            break;

        default:
            term = "Unknown";
        }

        float gpa = history[i].second;

        ui->historyTable->setItem(
            i,
            0,
            new QTableWidgetItem(term));

        ui->historyTable->setItem(
            i,
            1,
            new QTableWidgetItem(QString::number(gpa)));
    }
}