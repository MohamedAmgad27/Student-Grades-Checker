#pragma once

#include <QWidget>
#include "../src/Student.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class StudentDashboard;
}
QT_END_NAMESPACE

class StudentDashboard : public QWidget
{
    Q_OBJECT

public:
    explicit StudentDashboard(Student* student, QWidget *parent = nullptr);
    ~StudentDashboard();

private:
    Ui::StudentDashboard *ui;

    Student* currentStudent;

    void loadStudentData();
};