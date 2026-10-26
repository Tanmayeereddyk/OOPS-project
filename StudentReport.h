#ifndef STUDENTREPORT_H
#define STUDENTREPORT_H
#include <iostream>
#include <string>
#include "AttemptStorage.h"
enum class StudentReportStatus {
    validAttempt,
    noAttempt,
    invalidAttempt,
    studentNotFound,
    examNotFound,
    subjectNotFound
};
struct StudentExamSummary {
    int studentId = 0;
    std::string fullName;
    int rollNo = 0;
    std::string className;
    std::string section;
    int examId = 0;
    std::string examName;
    std::string subjectName;
    double maximumMarks = 0.0;
    bool attempted = false;
    StudentReportStatus status = StudentReportStatus::invalidAttempt;
    double marksObtained = 0.0;
    double percentage = 0.0;
    std::string grade;
    bool passed = false;
};
class StudentReport {
public:
    StudentReport();
    static StudentReportStatus getStudentExamSummary(
        int studentId, int examId, StudentExamSummary& summary,
        std::ostream& out = std::cout,
        const std::string& attemptFilePath = AttemptStorage::defaultFilePath());
<<<<<<< HEAD

    bool generateReport(int studentId, int examId, std::ostream& out = std::cout, const std::string& attemptFilePath = AttemptStorage::defaultFilePath()) const;
=======
    bool generateReport(int studentId, int examId,
                        std::ostream& out = std::cout,
                        const std::string& attemptFilePath = AttemptStorage::defaultFilePath()) const;
>>>>>>> 02e772a6c4a9b57230baf33aa5936688f6e01ba8
};
#endif