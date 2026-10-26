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
    bool generateReport(int studentId, int examId,
                        std::ostream& out = std::cout,
                        const std::string& attemptFilePath = AttemptStorage::defaultFilePath()) const;
};
#endif