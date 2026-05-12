#pragma once
#include "Student.hpp"
#include "Types.hpp"
#include <string>

#include <map>

#include <vector>

// Node structure declaration for the Binary Search Tree
struct TreeNode
{
    Student student;
    TreeNode *left;
    TreeNode *right;

    // Constructor ensuring the student is copied safely
    TreeNode(const Student &s) : student(s), left(nullptr), right(nullptr) {}
};

class GradeManager
{
private:
    TreeNode *root; // Root of the Student BST

    // Private helper functions for recursive BST operations
    TreeNode *insertHelper(TreeNode *node, const Student &newStudent);
    TreeNode *searchHelper(TreeNode *node, int id) const;

    // Deletion helpers
    TreeNode *deleteHelper(TreeNode *node, int id);
    TreeNode *findInOrderSuccessor(TreeNode *node) const; // Finds the in-order successor

    // Traversal and memory management
    void inorderTraversal(TreeNode *node) const;
    void destroyTree(TreeNode *node);


    // Habiba: Helper to collect students for the UI -->
    void collectStudentsInOrder(TreeNode *node, std::vector<Student*> &studentList) const;


public:
    // Constructor & Destructor
    GradeManager();
    ~GradeManager();

    // Core Student Operations (Using BST)
    void addStudent(int id, const string &name);

    // Returns a pointer so the student object can be modified in memory
    Student *searchStudent(int id) const;

    // Removes a student from the tree
    bool deleteStudent(int id);

    // Grade modifier: Finds the student and appends a GPA
    void addTermGPA(int studentId, Term term, float gpa);


    // Display operations (Terminal only)
    void displayAllStudents() const;

    void collectStudents(TreeNode* node, vector<Student*>& students) const;

    vector<Student*> getTopStudentsByCGPA(int topN = 10) const; //get top students in report page
    void getPassFailReport(int &passCount, int &failCount, float passingGPA = 2.0) const;
    map<string, int> getGradeDistribution() const;
    vector<Student*> getAllStudentsAsList() const;

    // Habiba: GUI Fetcher -->

    // // Display operations (Terminal only)
    // void displayAllStudents() const;

};