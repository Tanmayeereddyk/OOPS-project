#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H
#include <functional>
#include <string>
#include <vector>
#include "student.h"
class StudentManager {
private:
    std::vector<Student> students;   // list of all students
    int nextId;                      // next ID to give (starts at 101)
    std::string filePath;            // example: data/students.txt
    // Private helper functions
    int makeNewId();
    bool rollNoExists(int rollNo, int skipId) const;
    bool emailExists(const std::string& email, int skipId) const;
    bool checkStudent(const Student& student, int skipId, std::string& errorMessage) const;
    // Screens used by showMenu
    void addStudentMenu();
    void searchStudentMenu();
    void updateStudentMenu();
    void deleteStudentMenu(const std::function<bool(int)>& hasLinks);
    void sortStudentMenu();
public:
    StudentManager(const std::string& filePath);
    int addStudent(const Student& student);                 // returns new ID or -1
    Student* findById(int studentId);                       // nullptr if not found
    Student* findByRollNo(int rollNo);                      // nullptr if not found
    std::vector<Student> findByClassAndSection(const std::string& studentClass,
                                               const std::string& section) const;
    std::vector<Student> getStudentsByClassAndSection(const std::string& studentClass,
                                                      const std::string& section) const;
    bool updateStudent(int studentId, const Student& newStudent);
    bool deleteStudent(int studentId, const std::function<bool(int)>& hasLinks);
    bool studentExists(int studentId) const;
    int countStudents() const;
    void showAllStudents() const;
    // Takes a function pointer (sortByName or sortById)
    void sortStudents(bool (*compareFunction)(const Student&, const Student&));
    bool saveData() const;
    bool loadData();
    void showMenu(const std::function<bool(int)>& hasLinks);
};
// Free functions, used as function pointers
bool sortByName(const Student& a, const Student& b);
bool sortById(const Student& a, const Student& b);
#endif