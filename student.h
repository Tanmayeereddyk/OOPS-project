#ifndef STUDENT_H
#define STUDENT_H
#include <iostream>
#include <string>
class Student {
private:
    int studentId;
    int rollNo;
    std::string fullName;
    std::string className;
    std::string section;
    std::string email;
public:
    // Constructors
    Student();
    Student(int studentId, int rollNo, const std::string& fullName,
            const std::string& className, const std::string& section,
            const std::string& email);
    // Getters
    int getStudentId() const;
    int getRollNo() const;
    std::string getFullName() const;
    std::string getClassName() const;
    std::string getSection() const;
    std::string getEmail() const;
    // Setters
    void setStudentId(int studentId);
    void setRollNo(int rollNo);
    void setFullName(const std::string& fullName);
    void setClassName(const std::string& className);
    void setSection(const std::string& section);
    void setEmail(const std::string& email);
    // Display and file helpers
    void showStudent() const;
    std::string toLine() const;
    static bool fromLine(const std::string& line, Student& student);
    // Friend function for cout << student
    friend std::ostream& operator<<(std::ostream& os, const Student& student);
};
#endif