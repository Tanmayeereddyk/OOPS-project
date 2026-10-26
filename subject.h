#ifndef SUBJECT_H
#define SUBJECT_H
#include <iostream>
#include <string>
class Subject {
private:
    int subjectId;
    std::string subjectName;
public:
    // Constructors
    Subject();
    Subject(int subjectId, const std::string& subjectName);

    static int findSubjectIdByName(const std::string& subjectName);
    static void registerSubject(const Subject& subject);
    static void unregisterSubject(int subjectId);
    static void clearRegistry();

    // Getters
    int getSubjectId() const;
    std::string getSubjectName() const;
    // Setters
    void setSubjectId(int subjectId);
    void setSubjectName(const std::string& subjectName);
    // Display and file helpers
    void showSubject() const;
    std::string toLine() const;
    static bool fromLine(const std::string& line, Subject& subject);
    // Friend function for cout << subject
    friend std::ostream& operator<<(std::ostream& os, const Subject& subject);
};
#endif
