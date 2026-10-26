#include "ClassReport.h"
#include "StudentReport.h"
#include "studentmanager.h"
#include "Exam.h"

#include <sstream>
#include <vector>
#include <stdexcept>

ClassReport::ClassReport() {}

bool ClassReport::generateReport(int examId,
                                const std::string& className,
                                const std::string& section,
                                std::ostream& out,
                                const std::string& attemptFilePath) const {
    StudentManager studentManager("data/students.txt");
    studentManager.loadData();

    std::vector<Student> students = studentManager.findByClassAndSection(className, section);
    if (students.empty()) {
        out << "No students found in class " << className << " and section " << section << ".\n";
        return false;
    }

    try {
        Exam exam(examId);
        (void)exam;
    } catch (const std::exception& ex) {
        out << "Error loading exam " << examId << ": " << ex.what() << "\n";
        return false;
    }

    out << "\n================ CLASS REPORT ================\n";
    out << "Exam ID    : " << examId << "\n";
    out << "Class      : " << className << "\n";
    out << "Section    : " << section << "\n\n";

    int attemptedCount = 0;
    int validAttemptCount = 0;
    int notAttemptedCount = 0;
    int invalidCount = 0;
    int passedCount = 0;
    int failedCount = 0;
    double totalMarks = 0.0;
    double highestMarks = 0.0;
    double lowestMarks = 0.0;
    bool hasValidAttempt = false;

    for (const Student& student : students) {
        StudentExamSummary summary;
        std::ostringstream quiet;
        StudentReportStatus status = StudentReport::getStudentExamSummary(
            student.getStudentId(), examId, summary, quiet, attemptFilePath
        );

        out << "Student ID   : " << student.getStudentId() << "\n";
        out << "Student Name : " << student.getFullName() << "\n";
        out << "Roll No     : " << student.getRollNo() << "\n";

        if (status == StudentReportStatus::noAttempt) {
            out << "Marks       : Not attempted\n";
            out << "Percentage  : Not available\n";
            out << "Grade       : Not available\n";
            out << "Pass/Fail   : Not available\n\n";
            ++notAttemptedCount;
            continue;
        }

        if (status != StudentReportStatus::validAttempt) {
            if (summary.attempted) {
                ++attemptedCount;
            }
            out << "Marks       : "
                << (status == StudentReportStatus::subjectNotFound
                    ? "Subject record unavailable"
                    : "Invalid/unavailable result")
                << "\n";
            out << "Percentage  : Not available\n";
            out << "Grade       : Not available\n";
            out << "Pass/Fail   : Not available\n\n";
            ++invalidCount;
            continue;
        }

        ++attemptedCount;
        ++validAttemptCount;
        out << "Marks       : " << summary.marksObtained << " / " << summary.maximumMarks << "\n";
        out << "Percentage  : " << summary.percentage << "%\n";
        out << "Grade       : " << summary.grade << "\n";
        out << "Pass/Fail   : " << (summary.passed ? "Pass" : "Fail") << "\n\n";

        totalMarks += summary.marksObtained;
        if (!hasValidAttempt) {
            highestMarks = summary.marksObtained;
            lowestMarks = summary.marksObtained;
            hasValidAttempt = true;
        } else {
            if (summary.marksObtained > highestMarks) {
                highestMarks = summary.marksObtained;
            }
            if (summary.marksObtained < lowestMarks) {
                lowestMarks = summary.marksObtained;
            }
        }

        if (summary.passed) {
            ++passedCount;
        } else {
            ++failedCount;
        }
    }

    out << "\n=============== CLASS STATISTICS ===============\n";
    out << "Total students in class/section : " << students.size() << "\n";
    out << "Students who attempted         : " << attemptedCount << "\n";
    out << "Valid evaluated attempts       : " << validAttemptCount << "\n";
    out << "Students who did not attempt   : " << notAttemptedCount << "\n";
    out << "Invalid/unavailable results   : " << invalidCount << "\n";
    out << "Students who passed           : " << passedCount << "\n";
    out << "Students who failed           : " << failedCount << "\n";

    if (!hasValidAttempt) {
        out << "Average marks                 : Unavailable\n";
        out << "Highest marks                 : Unavailable\n";
        out << "Lowest marks                  : Unavailable\n";
    } else {
        const double averageMarks = totalMarks / validAttemptCount;
        out << "Average marks                 : " << averageMarks << "\n";
        out << "Highest marks                 : " << highestMarks << "\n";
        out << "Lowest marks                  : " << lowestMarks << "\n";
    }

    out << "==============================================\n";
    return true;
}
