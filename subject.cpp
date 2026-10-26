#include "subject.h"
#include "validation.h"
#include <cctype>
#include <vector>
//Small helper (only used inside this file)
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
std::vector<Subject>& subjectRegistry() {
    static std::vector<Subject> registry;
    return registry;
}
std::string normalizeSubjectName(const std::string& text) {
    std::string normalized = Validation::trimText(text);
    for (char& ch : normalized) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return normalized;
}
}
//Constructors
Subject::Subject() {
    subjectId = 0;
    subjectName = "";
}
Subject::Subject(int subjectId, const std::string& subjectName) {
    // Start blank, then let the setters accept only valid data
    this->subjectId = 0;
    this->subjectName = "";
    setSubjectId(subjectId);
    setSubjectName(subjectName);
}
void Subject::registerSubject(const Subject& subject) {
    if (subject.getSubjectId() <= 0 || subject.getSubjectName().empty()) {
        return;
    }
    std::vector<Subject>& registry = subjectRegistry();
    for (Subject& existing : registry) {
        if (existing.getSubjectId() == subject.getSubjectId()) {
            existing = subject;
            return;
        }
    }
    registry.push_back(subject);
}
void Subject::unregisterSubject(int subjectId) {
    std::vector<Subject>& registry = subjectRegistry();
    for (std::vector<Subject>::iterator it = registry.begin();
         it != registry.end(); ++it) {
        if (it->getSubjectId() == subjectId) {
            registry.erase(it);
            return;
        }
    }
}
void Subject::clearRegistry() {
    subjectRegistry().clear();
}
//Getters
int Subject::getSubjectId() const { return subjectId; }
std::string Subject::getSubjectName() const { return subjectName; }
//Setters (invalid values are ignored)
void Subject::setSubjectId(int subjectId) {
    if (subjectId > 0) {
        this->subjectId = subjectId;
    }
}
void Subject::setSubjectName(const std::string& subjectName) {
    std::string text = Validation::trimText(subjectName);
    if (!text.empty() && !Validation::hasDelimiter(text)) {
        this->subjectName = text;
    }
}
//Display
void Subject::showSubject() const {
    std::cout << *this << std::endl;
}
std::ostream& operator<<(std::ostream& os, const Subject& subject) {
    os << "ID: " << subject.subjectId
       << " | Subject: " << subject.subjectName;
    return os;
}
//File helpers
std::string Subject::toLine() const {
    // Example: 201|Mathematics
    return std::to_string(subjectId) + Validation::DELIMITER + subjectName;
}
bool Subject::fromLine(const std::string& line, Subject& subject) {
    // Split at the first '|'
    size_t pos = line.find(Validation::DELIMITER);
    if (pos == std::string::npos) {
        return false;   // no '|' in the line
    }
    std::string idText = Validation::trimText(line.substr(0, pos));
    std::string nameText = Validation::trimText(line.substr(pos + 1));
    // ID must be a number greater than 0
    int id = 0;
    if (!toNumber(idText, id) || id <= 0) {
        return false;
    }
    // Name must not be empty and must not contain another '|'
    if (nameText.empty() || Validation::hasDelimiter(nameText)) {
        return false;
    }
    // Everything is valid, so fill the subject
    subject = Subject(id, nameText);
    return true;
}
//to help in exam class
int Subject::findSubjectIdByName(const std::string& subjectName) {
    const std::string normalized = normalizeSubjectName(subjectName);
    const std::vector<Subject>& registry = subjectRegistry();

    for (const Subject& subject : registry) {
        if (normalizeSubjectName(subject.getSubjectName()) == normalized) {
            return subject.getSubjectId();
        }
    }
    return -1;
}