#include "studentlogindialog.h"
#include "ui_studentlogindialog.h"
#include <QMessageBox>

StudentLoginDialog::StudentLoginDialog(GradeManager* backendPtr,
                                       QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StudentLoginDialog), backend(backendPtr)
{
    ui->setupUi(this);

    // Connect the search button to our validation slot
    connect(ui->btn_search, &QPushButton::clicked, this, &StudentLoginDialog::on_btn_search_clicked);
    connect(ui->btn_back,
            &QPushButton::clicked,
            this,
            &StudentLoginDialog::on_btn_back_clicked);
}

StudentLoginDialog::~StudentLoginDialog()
{
    delete ui;
}
void StudentLoginDialog::on_btn_back_clicked()
{
    this->reject();
}

QString StudentLoginDialog::getEnteredID() const
{
    return ui->lineEdit_studentId->text();

}

void StudentLoginDialog::on_btn_search_clicked()
{
    QString enteredId = getEnteredID();

    // Basic Validation
    if (enteredId.trimmed().isEmpty()) {
        QMessageBox::warning(this,
                             "Input Error",
                             "Please enter a valid Student ID.");
        return;
    }

    // convert QString -> int
    int id = enteredId.toInt();

    // search in BST
    loggedStudent = backend->searchStudent(id);

    // if not found
    if(loggedStudent == nullptr)
    {
        QMessageBox::warning(this,
                             "Not Found",
                             "Student does not exist");
        return;
    }


    accept();
}
Student* StudentLoginDialog::getLoggedStudent() const
{
    return loggedStudent;
}