#ifndef SUBJECTMANAGER_H
#define SUBJECTMANAGER_H
#include <functional>
#include <string>
#include <vector>
#include "Subject.h"
class SubjectManager {
private:
    std::vector<Subject> subjects;   // list of all subjects
    int nextId;                      // next ID to give (starts at 201)
    std::string filePath;            // example: data/subjects.txt
    // Private helper functions
    int makeNewId();
    bool nameExists(const std::string& name, int skipId) const;
    bool checkSubject(const Subject& subject, int skipId, std::string& errorMessage) const;
    // Screens used by showMenu
    void addSubjectMenu();
    void searchSubjectMenu();
    void updateSubjectMenu();
    void deleteSubjectMenu(const std::function<bool(int)>& hasLinks);
public:
    SubjectManager(const std::string& filePath);
    int addSubject(const Subject& subject);                 // returns new ID or -1
    Subject* findById(int subjectId);                       // nullptr if not found
    bool updateSubject(int subjectId, const Subject& newSubject);
    bool deleteSubject(int subjectId, const std::function<bool(int)>& hasLinks);
    bool subjectExists(int subjectId) const;
    int countSubjects() const;
    void showAllSubjects() const;
    bool saveData() const;
    bool loadData();
    void showMenu(const std::function<bool(int)>& hasLinks);
};
#endif