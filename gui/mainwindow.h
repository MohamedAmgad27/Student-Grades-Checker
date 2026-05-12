#pragma once

#include <QMainWindow>
#include "../src/GradeManager.hpp"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(GradeManager* manager,
                        QWidget *parent = nullptr);
    ~MainWindow();
void setGradeManager(GradeManager* manager);
private slots:

    void onLecturerTabChanged(int index);
    void handleAddCourse();
    void handleRecordGrade();



private:
    Ui::MainWindow *ui; // This connects to your .ui file!
GradeManager* backend;
};