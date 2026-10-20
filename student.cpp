#include "Student.h"
#include "Validation.h"
#include <sstream>
#include <vector>
//helpers functions (only used inside this file)
namespace {
// Converts text to a whole number. Returns false if the text is not a number.
bool toNumber(const std::string& text, int& number) {
    try {
        size_t used = 0;
        number = std::stoi(text, &used);
        return used == text.size();   // rejects things like "12abc"
    } catch (...) {
        return false;
    }
}
// Text is valid if it is not empty and does not contain '|'
bool isGoodText(const std::string& text) {
    return !text.empty() && !Validation::hasDelimiter(text);
}
} 
//Constructors
Student::Student() {
    studentId = 0;
    rollNo = 0;
    fullName = "";
    className = "";
    section = "";
    email = "";
}
Student::Student(int studentId, int rollNo, const std::string& fullName,
                 const std::string& className, const std::string& section,
                 const std::string& email) {
    this->studentId = 0;
    this->rollNo = 0;
    this->fullName = "";
    this->className = "";
    this->section = "";
    this->email = "";
    setStudentId(studentId);
    setRollNo(rollNo);
    setFullName(fullName);
    setClassName(className);
    setSection(section);
    setEmail(email);
}
//Getters
int Student::getStudentId() const { return studentId; }
int Student::getRollNo() const { return rollNo; }
std::string Student::getFullName() const { return fullName; }
std::string Student::getClassName() const { return className; }
std::string Student::getSection() const { return section; }
std::string Student::getEmail() const { return email; }
//Setters
void Student::setStudentId(int studentId) {
    if (studentId > 0) {
        this->studentId = studentId;
    }
}
void Student::setRollNo(int rollNo) {
    if (rollNo > 0) {
        this->rollNo = rollNo;
    }
}
void Student::setFullName(const std::string& fullName) {
    std::string text = Validation::trimText(fullName);
    if (isGoodText(text)) {
        this->fullName = text;
    }
}
void Student::setClassName(const std::string& className) {
    std::string text = Validation::trimText(className);
    if (isGoodText(text)) {
        this->className = text;
    }
}
void Student::setSection(const std::string& section) {
    std::string text = Validation::trimText(section);
    if (isGoodText(text)) {
        this->section = text;
    }
}
void Student::setEmail(const std::string& email) {
    std::string text = Validation::trimText(email);
    if (Validation::isValidEmail(text)) {
        this->email = text;
    }
}
//Display
void Student::showStudent() const {
    std::cout << *this << std::endl;
}
std::ostream& operator<<(std::ostream& os, const Student& student) {
    os << "ID: " << student.studentId
       << " | Roll No: " << student.rollNo
       << " | Name: " << student.fullName
       << " | Class: " << student.className
       << " | Section: " << student.section
       << " | Email: " << student.email;
    return os;
}
//File helpers
std::string Student::toLine() const {
    std::ostringstream line;
    line << studentId << Validation::DELIMITER
         << rollNo << Validation::DELIMITER
         << fullName << Validation::DELIMITER
         << className << Validation::DELIMITER
         << section << Validation::DELIMITER
         << email;
    return line.str();
}
bool Student::fromLine(const std::string& line, Student& student) {
    // Split the line into parts at every '|'
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string part;
    while (std::getline(stream, part, Validation::DELIMITER)) {
        parts.push_back(Validation::trimText(part));
    }
    if (!line.empty() && line.back() == Validation::DELIMITER) {
        parts.push_back("");
    }
    // A valid line has exactly 6 parts
    if (parts.size() != 6) {
        return false;
    }
    int id = 0;
    int roll = 0;
    if (!toNumber(parts[0], id) || !toNumber(parts[1], roll)) {
        return false;
    }
    if (id <= 0 || roll <= 0) {
        return false;
    }
    if (!isGoodText(parts[2]) || !isGoodText(parts[3]) || !isGoodText(parts[4])) {
        return false;
    }
    if (!Validation::isValidEmail(parts[5])) {
        return false;
    }
    // Everything is valid, so fill the student
    student = Student(id, roll, parts[2], parts[3], parts[4], parts[5]);
    return true;
}