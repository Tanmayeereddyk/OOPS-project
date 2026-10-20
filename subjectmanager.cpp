#include "SubjectManager.h"
#include "Validation.h"
#include <filesystem>
#include <fstream>
#include <iostream>
//Small constants
namespace {
const int START_ID = 201;
const std::string ID_HEADER = "NEXT_ID|";
}
//Constructor
SubjectManager::SubjectManager(const std::string& filePath) {
    this->filePath = filePath;
    nextId = START_ID;
}
//Private helpers
int SubjectManager::makeNewId() {
    int id = nextId;
    nextId++;
    return id;
}
bool SubjectManager::nameExists(const std::string& name, int skipId) const {
    for (size_t i = 0; i < subjects.size(); i++) {
        if (Validation::sameIgnoreCase(subjects[i].getSubjectName(), name)
            && subjects[i].getSubjectId() != skipId) {
            return true;
        }
    }
    return false;
}
bool SubjectManager::checkSubject(const Subject& subject, int skipId, std::string& errorMessage) const {
    // The setter ignores bad values, so an empty name means bad input
    if (subject.getSubjectName().empty()) {
        errorMessage = "Subject name cannot be empty or contain '|'.";
        return false;
    }
    if (nameExists(subject.getSubjectName(), skipId)) {
        errorMessage = "This subject name already exists.";
        return false;
    }
    return true;
}
//Add / find / update / delete
int SubjectManager::addSubject(const Subject& subject) {
    std::string error;
    if (!checkSubject(subject, 0, error)) {
        std::cout << "Cannot add subject: " << error << "\n";
        return -1;
    }
    Subject newSubject = subject;
    newSubject.setSubjectId(makeNewId());   // manager gives the ID
    subjects.push_back(newSubject);
    return newSubject.getSubjectId();
}
Subject* SubjectManager::findById(int subjectId) {
    for (size_t i = 0; i < subjects.size(); i++) {
        if (subjects[i].getSubjectId() == subjectId) {
            return &subjects[i];
        }
    }
    return nullptr;
}
bool SubjectManager::updateSubject(int subjectId, const Subject& newSubject) {
    for (size_t i = 0; i < subjects.size(); i++) {
        if (subjects[i].getSubjectId() == subjectId) {
            Subject updated = newSubject;
            updated.setSubjectId(subjectId);   // keep the same ID
            std::string error;
            if (!checkSubject(updated, subjectId, error)) {   // skip this subject itself
                std::cout << "Cannot update subject: " << error << "\n";
                return false;
            }
            subjects[i] = updated;
            return true;
        }
    }
    std::cout << "Subject with ID " << subjectId << " not found.\n";
    return false;
}
bool SubjectManager::deleteSubject(int subjectId, const std::function<bool(int)>& hasLinks) {
    for (size_t i = 0; i < subjects.size(); i++) {
        if (subjects[i].getSubjectId() == subjectId) {
            // Check linked exams first
            if (hasLinks && hasLinks(subjectId)) {
                std::cout << "Cannot delete: this subject is linked to an exam.\n";
                return false;
            }
            subjects.erase(subjects.begin() + i);
            return true;
        }
    }
    std::cout << "Subject with ID " << subjectId << " not found.\n";
    return false;
}
// Info and display
bool SubjectManager::subjectExists(int subjectId) const {
    for (size_t i = 0; i < subjects.size(); i++) {
        if (subjects[i].getSubjectId() == subjectId) {
            return true;
        }
    }
    return false;
}
int SubjectManager::countSubjects() const {
    return static_cast<int>(subjects.size());
}
void SubjectManager::showAllSubjects() const {
    if (subjects.empty()) {
        std::cout << "No subjects found.\n";
        return;
    }
    for (size_t i = 0; i < subjects.size(); i++) {
        std::cout << subjects[i] << std::endl;
    }
}
//File handling
bool SubjectManager::saveData() const {
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
    for (size_t i = 0; i < subjects.size(); i++) {
        file << subjects[i].toLine() << "\n";     // one subject per line
    }
    return true;
}
bool SubjectManager::loadData() {
    subjects.clear();
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
        // Subject line
        Subject s;
        std::string error;
        if (!Subject::fromLine(line, s) || subjectExists(s.getSubjectId())
            || !checkSubject(s, 0, error)) {
            std::cout << "Skipping a bad or duplicate line in " << filePath << "\n";
            continue;
        }
        subjects.push_back(s);
        if (s.getSubjectId() > highestId) {
            highestId = s.getSubjectId();
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
void SubjectManager::addSubjectMenu() {
    std::cout << "\n--- Add Subject ---\n";
    std::string name = Validation::readText("Subject name: ");
    int id = addSubject(Subject(0, name));
    if (id != -1) {
        std::cout << "Subject added with ID " << id << ".\n";
    }
}
void SubjectManager::searchSubjectMenu() {
    std::cout << "\n--- Search Subject ---\n";
    int id = Validation::readNumber("Enter subject ID: ", 1, 999999);
    // Use the pointer right away, never store it
    Subject* found = findById(id);
    if (found != nullptr) {
        std::cout << *found << std::endl;
    } else {
        std::cout << "Subject not found.\n";
    }
}
void SubjectManager::updateSubjectMenu() {
    std::cout << "\n--- Update Subject ---\n";
    int id = Validation::readNumber("Enter subject ID to update: ", 1, 999999);
    Subject* current = findById(id);
    if (current == nullptr) {
        std::cout << "Subject not found.\n";
        return;
    }
    std::cout << "Current details: " << *current << std::endl;
    std::string newName = Validation::readText("New subject name: ");
    if (updateSubject(id, Subject(0, newName))) {
        std::cout << "Subject updated.\n";
    }
}
void SubjectManager::deleteSubjectMenu(const std::function<bool(int)>& hasLinks) {
    std::cout << "\n--- Delete Subject ---\n";
    int id = Validation::readNumber("Enter subject ID to delete: ", 1, 999999);
    if (deleteSubject(id, hasLinks)) {
        std::cout << "Subject deleted.\n";
    }
}
void SubjectManager::showMenu(const std::function<bool(int)>& hasLinks) {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n===== SUBJECT MANAGEMENT =====\n";
        std::cout << "1. Add subject\n";
        std::cout << "2. Search subject\n";
        std::cout << "3. Update subject\n";
        std::cout << "4. Delete subject\n";
        std::cout << "5. Show all subjects\n";
        std::cout << "0. Back\n";
        choice = Validation::readNumber("Enter choice: ", 0, 5);
        if (choice == 1) addSubjectMenu();
        else if (choice == 2) searchSubjectMenu();
        else if (choice == 3) updateSubjectMenu();
        else if (choice == 4) deleteSubjectMenu(hasLinks);
        else if (choice == 5) showAllSubjects();
    }
}