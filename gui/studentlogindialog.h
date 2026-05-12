#pragma once

#include <QDialog>
#include <QString>
#include "../src/GradeManager.hpp"

namespace Ui { class StudentLoginDialog; }

class StudentLoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StudentLoginDialog(GradeManager* backendPtr,
                                QWidget *parent = nullptr);
    ~StudentLoginDialog();
      GradeManager* backend;

    // Getter so the main window can retrieve the entered ID
    QString getEnteredID() const;

    Student* getLoggedStudent() const;

private slots:
    void on_btn_search_clicked();
  void on_btn_back_clicked();
private:
    Ui::StudentLoginDialog *ui;


  Student* loggedStudent = nullptr;
};