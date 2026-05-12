#include "GradeManager.hpp"
#include <iostream>
#include <map>
#include <algorithm>
using namespace std;

GradeManager::GradeManager() : root(nullptr) {}

GradeManager::~GradeManager()
{
    // clean up memory when the program closes
    destroyTree(root);
}

void GradeManager::destroyTree(TreeNode *node)
{
    if (node != nullptr)
    {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}




///////////adding student ////////////


void GradeManager::addStudent(int id, const string &name)
{
    Student newStudent(id, name);
    root = insertHelper(root, newStudent);
}


/////// search student ///////////

Student *GradeManager::searchStudent(int id) const
{
    TreeNode *result = searchHelper(root, id);


    if (result != nullptr)
    {
        // cast away const so the teammate's code can modify the student's grades
        return const_cast<Student *>(&(result->student));
    }
    return nullptr;
}

/////delete student//////

bool GradeManager::deleteStudent(int id)
{

    // check if student exists first
    if (searchStudent(id) == nullptr)
    {
        return false;
    }

    root = deleteHelper(root, id);
    return true;
}

////////adding gpa
void GradeManager::addTermGPA(int studentId, Term term, float gpa)
{
    Student *foundStudent = searchStudent(studentId);


    if (foundStudent != nullptr)
    {
        foundStudent->addTermGPA(term, gpa);
        cout << "GPA recorded for ID: " << studentId << "\n";
    }
    else
    {
        cout << "Error: Student ID " << studentId << " not found.\n";
    }
}

// void GradeManager::displayAllStudents() const
// {
//     if (root == nullptr)
//     {
//         cout << "No students in the system.\n";
//         return;
//     }

//     cout << "--- Student Directory ---\n";
//     inorderTraversal(root);
//     cout << "-------------------------\n";
// }

TreeNode *GradeManager::insertHelper(TreeNode *node, const Student &newStudent)
{
    // found an empty leaf, create the node here
    if (node == nullptr)
    {
        return new TreeNode(newStudent);
    }

    // sort by ID
    if (newStudent.getID() < node->student.getID())
    {
        node->left = insertHelper(node->left, newStudent);
    }
    else if (newStudent.getID() > node->student.getID())
    {
        node->right = insertHelper(node->right, newStudent);
    }

    // duplicates are ignored automatically
    return node;
}

TreeNode *GradeManager::searchHelper(TreeNode *node, int id) const
{
    // return if found or if we hit a dead end
    if (node == nullptr || node->student.getID() == id)
    {
        return node;
    }

    // navigate left or right based on ID
    if (id < node->student.getID())
    {
        return searchHelper(node->left, id);
    }
    return searchHelper(node->right, id);
}

TreeNode *GradeManager::deleteHelper(TreeNode *node, int id)
{
    if (node == nullptr)
        return node;

    // locate the node
    if (id < node->student.getID())
    {
        node->left = deleteHelper(node->left, id);
    }
    else if (id > node->student.getID())
    {
        node->right = deleteHelper(node->right, id);
    }
    else
    {
        // node found. handle cases where it has 0 or 1 child
        if (node->left == nullptr)
        {
            TreeNode *temp = node->right;
            delete node;
            return temp;
        }
        else if (node->right == nullptr)
        {
            TreeNode *temp = node->left;
            delete node;
            return temp;
        }

        // node has 2 children. replace it with the in-order successor
        TreeNode *temp = findInOrderSuccessor(node->right);
        node->student = temp->student;
        node->right = deleteHelper(node->right, temp->student.getID());
    }
    return node;
}

TreeNode *GradeManager::findInOrderSuccessor(TreeNode *node) const
{
    TreeNode *current = node;
    // go as far left as possible to find the smallest value
    while (current && current->left != nullptr)
    {
        current = current->left;
    }
    return current;
}

void GradeManager::inorderTraversal(TreeNode *node) const
{
    if (node != nullptr)
    {
        inorderTraversal(node->left);
        // node->student.displayStudentInfo();
        inorderTraversal(node->right);
    }
}

////convert bst to list/////
// Get all students as a list (for sorting/filtering)
vector<Student*> GradeManager::getAllStudentsAsList() const {
    vector<Student*> students;
    collectStudents(root, students);
    return students;
}

void GradeManager::collectStudents(TreeNode* node, vector<Student*>& students) const {
    if (node != nullptr) {
        collectStudents(node->left, students);
        students.push_back(const_cast<Student*>(&(node->student)));
        collectStudents(node->right, students);
    }
}

// Get top N students by CGPA
vector<Student*> GradeManager::getTopStudentsByCGPA(int topN) const {
    vector<Student*> allStudents = getAllStudentsAsList(); //Get them as lists first

    // Sort by CGPA
    sort(allStudents.begin(), allStudents.end(), //used stl sort
         [](const Student* a, const Student* b) {
             return a->calculateCumulativeGPA() > b->calculateCumulativeGPA();
         });

    // Return top N
    if (allStudents.size() > topN) {
        allStudents.resize(topN); //resize vector to contain on N students
    }

    return allStudents;
}

// Get pass/fail counts
void GradeManager::getPassFailReport(int &passCount, int &failCount, float passingGPA) const {
    passCount = 0;
    failCount = 0;

    vector<Student*> allStudents = getAllStudentsAsList();

    for (Student* student : allStudents) //Loop through each student
    {
        float cgpa = student->calculateCumulativeGPA(); //get each student gpa
        if (cgpa >= passingGPA) {
            passCount++;
        } else {
            failCount++;
        }
    }
}

// Get grade distribution (A, B, C, D, F) //to draw histogram
map<string, int> GradeManager::getGradeDistribution() const {
    map<string, int> distribution;  //mapping to store Grade and count
    distribution["A"] = 0;
    distribution["B"] = 0;
    distribution["C"] = 0;
    distribution["D"] = 0;
    distribution["F"] = 0;

    vector<Student*> allStudents = getAllStudentsAsList();

    for (Student* student : allStudents) //loop through each student and add its grade in its specific place
    {
        float cgpa = student->calculateCumulativeGPA();

        if (cgpa >= 3.7) {
            distribution["A"]++;
        } else if (cgpa >= 3.0) {
            distribution["B"]++;
        } else if (cgpa >= 2.0) {
            distribution["C"]++;
        } else if (cgpa >= 1.0) {
            distribution["D"]++;
        } else {
            distribution["F"]++;
        }
    }

    return distribution;

}