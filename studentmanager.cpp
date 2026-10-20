#include "StudentManager.h"
#include "Validation.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
//Small helpers
namespace {
const int START_ID = 101;
const std::string ID_HEADER = "NEXT_ID|";
// Keeps asking until the user types a valid email
std::string readEmail() {
    while (true) {
        std::string email = Validation::readText("Email: ");
        if (Validation::isValidEmail(email)) {
            return email;
        }
        std::cout << "Invalid email. It needs one '@' and a '.' after it.\n";
    }
}
// Asks the user for all student details (ID is not asked, the manager makes it)
Student readStudentDetails() {
    int rollNo = Validation::readNumber("Roll number: ", 1, 999999);
    std::string name = Validation::readText("Full name: ");
    std::string className = Validation::readText("Class: ");
    std::string section = Validation::readText("Section: ");
    std::string email = readEmail();
    return Student(0, rollNo, name, className, section, email);
}
}
//Constructor
StudentManager::StudentManager(const std::string& filePath) {
    this->filePath = filePath;
    nextId = START_ID;
}
//Private helpers
int StudentManager::makeNewId() {
    int id = nextId;
    nextId++;
    return id;
}
bool StudentManager::rollNoExists(int rollNo, int skipId) const {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getRollNo() == rollNo && students[i].getStudentId() != skipId) {
            return true;
        }
    }
    return false;
}
bool StudentManager::emailExists(const std::string& email, int skipId) const {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getEmail() == email && students[i].getStudentId() != skipId) {
            return true;
        }
    }
    return false;
}
bool StudentManager::checkStudent(const Student& student, int skipId, std::string& errorMessage) const {
    // The setters ignore bad values, so a blank or zero field means bad input
    if (student.getRollNo() <= 0) {
        errorMessage = "Roll number must be greater than 0.";
        return false;
    }
    if (student.getFullName().empty()) {
        errorMessage = "Full name cannot be empty or contain '|'.";
        return false;
    }
    if (student.getClassName().empty()) {
        errorMessage = "Class cannot be empty or contain '|'.";
        return false;
    }
    if (student.getSection().empty()) {
        errorMessage = "Section cannot be empty or contain '|'.";
        return false;
    }
    if (student.getEmail().empty()) {
        errorMessage = "Email is missing or invalid.";
        return false;
    }
    if (rollNoExists(student.getRollNo(), skipId)) {
        errorMessage = "This roll number already exists.";
        return false;
    }
    if (emailExists(student.getEmail(), skipId)) {
        errorMessage = "This email already exists.";
        return false;
    }
    return true;
}
//Add / find / update / delete
int StudentManager::addStudent(const Student& student) {
    std::string error;
    if (!checkStudent(student, 0, error)) {
        std::cout << "Cannot add student: " << error << "\n";
        return -1;
    }
    Student newStudent = student;
    newStudent.setStudentId(makeNewId());   // manager gives the ID
    students.push_back(newStudent);
    return newStudent.getStudentId();
}
Student* StudentManager::findById(int studentId) {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getStudentId() == studentId) {
            return &students[i];
        }
    }
    return nullptr;
}
Student* StudentManager::findByRollNo(int rollNo) {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getRollNo() == rollNo) {
            return &students[i];
        }
    }
    return nullptr;
}
bool StudentManager::updateStudent(int studentId, const Student& newStudent) {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getStudentId() == studentId) {
            Student updated = newStudent;
            updated.setStudentId(studentId);   // keep the same ID
            std::string error;
            if (!checkStudent(updated, studentId, error)) {   // skip this student itself
                std::cout << "Cannot update student: " << error << "\n";
                return false;
            }
            students[i] = updated;
            return true;
        }
    }
    std::cout << "Student with ID " << studentId << " not found.\n";
    return false;
}
bool StudentManager::deleteStudent(int studentId, const std::function<bool(int)>& hasLinks) {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getStudentId() == studentId) {
            // Check linked attempts/results first
            if (hasLinks && hasLinks(studentId)) {
                std::cout << "Cannot delete: this student has exam attempts or results.\n";
                return false;
            }
            students.erase(students.begin() + i);
            return true;
        }
    }
    std::cout << "Student with ID " << studentId << " not found.\n";
    return false;
}
//Info and display
bool StudentManager::studentExists(int studentId) const {
    for (size_t i = 0; i < students.size(); i++) {
        if (students[i].getStudentId() == studentId) {
            return true;
        }
    }
    return false;
}
int StudentManager::countStudents() const {
    return static_cast<int>(students.size());
}
void StudentManager::showAllStudents() const {
    if (students.empty()) {
        std::cout << "No students found.\n";
        return;
    }
    for (size_t i = 0; i < students.size(); i++) {
        std::cout << students[i] << std::endl;
    }
}
//Sorting (function pointer)
void StudentManager::sortStudents(bool (*compareFunction)(const Student&, const Student&)) {
    std::sort(students.begin(), students.end(), compareFunction);
}
bool sortByName(const Student& a, const Student& b) {
    return a.getFullName() < b.getFullName();
}
bool sortById(const Student& a, const Student& b) {
    return a.getStudentId() < b.getStudentId();
}
//File handling
bool StudentManager::saveData() const {
    // Make the folder (for example "data") if it does not exist
    std::filesystem::path folder = std::filesystem::path(filePath).parent_path();
    if (!folder.empty()) {
        std::error_code ec;
        std::filesystem::create_directories(folder, ec);
    }
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cout << "Error: could not open " << filePath << " for saving.\n";
        return false;
    }
    file << ID_HEADER << nextId << "\n";          // first line: NEXT_ID|n
    for (size_t i = 0; i < students.size(); i++) {
        file << students[i].toLine() << "\n";     // one student per line
    }
    return true;
}
bool StudentManager::loadData() {
    students.clear();
    nextId = START_ID;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        // File does not exist yet (first run). This is normal, start empty.
        return true;
    }
    int savedNextId = 0;
    int highestId = 0;
    std::string line;
    while (std::getline(file, line)) {
        line = Validation::trimText(line);
        if (line.empty()) {
            continue;
        }
        // First line: NEXT_ID|n
        if (line.rfind(ID_HEADER, 0) == 0) {
            try {
                savedNextId = std::stoi(line.substr(ID_HEADER.size()));
            } catch (...) {
                savedNextId = 0;   // bad header, will be fixed below
            }
            continue;
        }
        // Student line
        Student s;
        std::string error;
        if (!Student::fromLine(line, s) || studentExists(s.getStudentId())
            || !checkStudent(s, 0, error)) {
            std::cout << "Skipping a bad or duplicate line in " << filePath << "\n";
            continue;
        }
        students.push_back(s);
        if (s.getStudentId() > highestId) {
            highestId = s.getStudentId();
        }
    }
    // Fix nextId if the header is missing, wrong or too small
    nextId = savedNextId;
    if (nextId <= highestId) {
        nextId = highestId + 1;
    }
    if (nextId < START_ID) {
        nextId = START_ID;
    }
    return true;
}
//Menu screens
void StudentManager::addStudentMenu() {
    std::cout << "\n--- Add Student ---\n";
    Student s = readStudentDetails();
    int id = addStudent(s);
    if (id != -1) {
        std::cout << "Student added with ID " << id << ".\n";
    }
}
void StudentManager::searchStudentMenu() {
    std::cout << "\n--- Search Student ---\n";
    std::cout << "1. Search by ID\n2. Search by roll number\n";
    int choice = Validation::readNumber("Enter choice: ", 1, 2);
    Student* found = nullptr;
    if (choice == 1) {
        int id = Validation::readNumber("Enter student ID: ", 1, 999999);
        found = findById(id);
    } else {
        int roll = Validation::readNumber("Enter roll number: ", 1, 999999);
        found = findByRollNo(roll);
    }
    // Use the pointer right away, never store it
    if (found != nullptr) {
        std::cout << *found << std::endl;
    } else {
        std::cout << "Student not found.\n";
    }
}
void StudentManager::updateStudentMenu() {
    std::cout << "\n--- Update Student ---\n";
    int id = Validation::readNumber("Enter student ID to update: ", 1, 999999);
    Student* current = findById(id);
    if (current == nullptr) {
        std::cout << "Student not found.\n";
        return;
    }
    std::cout << "Current details: " << *current << std::endl;
    std::cout << "Enter the new details:\n";
    Student newDetails = readStudentDetails();
    if (updateStudent(id, newDetails)) {
        std::cout << "Student updated.\n";
    }
}
void StudentManager::deleteStudentMenu(const std::function<bool(int)>& hasLinks) {
    std::cout << "\n--- Delete Student ---\n";
    int id = Validation::readNumber("Enter student ID to delete: ", 1, 999999);
    if (deleteStudent(id, hasLinks)) {
        std::cout << "Student deleted.\n";
    }
}
void StudentManager::sortStudentMenu() {
    std::cout << "\n--- Sort Students ---\n";
    std::cout << "1. Sort by name\n2. Sort by ID\n";
    int choice = Validation::readNumber("Enter choice: ", 1, 2);
    if (choice == 1) {
        sortStudents(sortByName);   // passing a function pointer
    } else {
        sortStudents(sortById);
    }
    showAllStudents();
}
void StudentManager::showMenu(const std::function<bool(int)>& hasLinks) {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n===== STUDENT MANAGEMENT =====\n";
        std::cout << "1. Add student\n";
        std::cout << "2. Search student\n";
        std::cout << "3. Update student\n";
        std::cout << "4. Delete student\n";
        std::cout << "5. Show all students\n";
        std::cout << "6. Sort students\n";
        std::cout << "0. Back\n";
        choice = Validation::readNumber("Enter choice: ", 0, 6);
        if (choice == 1) addStudentMenu();
        else if (choice == 2) searchStudentMenu();
        else if (choice == 3) updateStudentMenu();
        else if (choice == 4) deleteStudentMenu(hasLinks);
        else if (choice == 5) showAllStudents();
        else if (choice == 6) sortStudentMenu();
    }
}