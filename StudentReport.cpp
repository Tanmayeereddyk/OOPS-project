#include "StudentReport.h"
#include "studentmanager.h"
#include "subjectmanager.h"
#include "GradingSystem.h"
#include "Evaluation.h"
#include "Exam.h"
#include "examattempt.h"
#include "Answer.h"
#include <cerrno>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <sys/stat.h>
#include <vector>
namespace {
enum class AttemptLookupStatus {
    found,
    notFound,
    invalid
};
struct SavedAttempt {
    int attemptId = 0;
    bool started = false;
    bool submitted = false;
    int currentQuestionIndex = 0;
    std::vector<Answer> answers;
};
bool hasNoTrailingData(std::istringstream& line) {
    line >> std::ws;
    return line.eof();
}
AttemptLookupStatus loadAttemptForStudent(const std::string& filePath,
                                          int studentId,
                                          int examId,
                                          SavedAttempt& saved,
                                          bool& matchingAttemptSeen) {
    if (filePath.empty()) {
        return AttemptLookupStatus::invalid;
    }
    std::ifstream input(filePath.c_str());
    if (!input.is_open()) {
        struct stat fileInfo;
        if (stat(filePath.c_str(), &fileInfo) != 0 && errno == ENOENT) {
            return AttemptLookupStatus::notFound;
        }
        return AttemptLookupStatus::invalid;
    }
    bool insideAttempt = false;
    bool currentMatches = false;
    bool matchingAttemptFound = false;
    SavedAttempt current;
    std::string line;
    while (std::getline(input, line)) {
        std::istringstream record(line);
        std::string marker;
        if (!(record >> marker)) {
            return AttemptLookupStatus::invalid;
        }
        if (marker == "ATTEMPT") {
            if (insideAttempt) {
                return AttemptLookupStatus::invalid;
            }
            int startedFlag = -1;
            int submittedFlag = -1;
            int recordStudentId = 0;
            int recordExamId = 0;
            if (!(record >> current.attemptId >> recordStudentId >> recordExamId >> startedFlag >> submittedFlag >> current.currentQuestionIndex)) {
                return AttemptLookupStatus::invalid;
            }
            currentMatches = recordStudentId == studentId && recordExamId == examId;
            matchingAttemptSeen = matchingAttemptSeen || currentMatches;
            if (!hasNoTrailingData(record) ||
                current.attemptId <= 0 ||
                recordStudentId <= 0 ||
                recordExamId <= 0 ||
                (startedFlag != 0 && startedFlag != 1) ||
                (submittedFlag != 0 && submittedFlag != 1) ||
                current.currentQuestionIndex < 0) {
                return AttemptLookupStatus::invalid;
            }
            current.started = startedFlag == 1;
            current.submitted = submittedFlag == 1;
            current.answers.clear();
            insideAttempt = true;
            continue;
        }
        if (marker == "ANSWER") {
            int questionId = 0;
            int evaluatedFlag = -1;
            double marksAwarded = 0.0;
            std::string response;
            if (!insideAttempt || !(record >> questionId)) {
                return AttemptLookupStatus::invalid;
            }
            record >> std::ws;
            if (record.peek() != '"' ||
                !(record >> std::quoted(response) >> marksAwarded >> evaluatedFlag) ||
                !hasNoTrailingData(record) ||
                questionId <= 0 ||
                response.empty() ||
                !std::isfinite(marksAwarded) ||
                marksAwarded < 0.0 ||
                (evaluatedFlag != 0 && evaluatedFlag != 1)) {
                return AttemptLookupStatus::invalid;
            }

            if (currentMatches) {
                current.answers.push_back(
                    Answer(questionId, response, marksAwarded, evaluatedFlag == 1)
                );
            }
            continue;
        }
        if (marker == "END_ATTEMPT") {
            if (!insideAttempt || !hasNoTrailingData(record)) {
                return AttemptLookupStatus::invalid;
            }
            if (currentMatches) {
                if (matchingAttemptFound) {
                    return AttemptLookupStatus::invalid;
                }
                matchingAttemptFound = true;
                saved = current;
            }
            insideAttempt = false;
            currentMatches = false;
            continue;
        }
        return AttemptLookupStatus::invalid;
    }
    if (input.bad() || insideAttempt) {
        return AttemptLookupStatus::invalid;
    }
    return matchingAttemptFound? AttemptLookupStatus::found: AttemptLookupStatus::notFound;
}
bool validateRecordedMarks(const SavedAttempt& saved, const Exam& exam) {
    if (!saved.started || !saved.submitted || saved.answers.empty()) {
        return false;
    }
    std::vector<int> seenQuestionIds;
    const auto& questions = exam.getQuestions();
    if ((questions.empty() && saved.currentQuestionIndex != 0) ||
        (!questions.empty() &&
         static_cast<std::size_t>(saved.currentQuestionIndex) >= questions.size())) {
        return false;
    }
    for (const Answer& answer : saved.answers) {
        if (!answer.evaluated) {
            return false;
        }
        for (int seenId : seenQuestionIds) {
            if (seenId == answer.questionId) {
                return false;
            }
        }
        const Question* matchingQuestion = nullptr;
        for (const auto& question : questions) {
            if (question->getQuestionId() == answer.questionId) {
                matchingQuestion = question.get();
                break;
            }
        }
        if (matchingQuestion == nullptr ||
            answer.marksAwarded > matchingQuestion->getMaxMarks()) {
            return false;
        }
        seenQuestionIds.push_back(answer.questionId);
    }
    return true;
}
} // namespace
StudentReport::StudentReport() {}
StudentReportStatus StudentReport::getStudentExamSummary(
    int studentId,
    int examId,
    StudentExamSummary& summary,
    std::ostream& out,
    const std::string& attemptFilePath) {
    (void)out;
    summary = StudentExamSummary();
    StudentManager studentManager("data/students.txt");
    studentManager.loadData();
    Student* student = studentManager.findById(studentId);
    if (student == nullptr) {
        return StudentReportStatus::studentNotFound;
    }
    std::unique_ptr<Exam> exam;
    try {
        exam.reset(new Exam(examId));
    } catch (const std::exception&) {
        return StudentReportStatus::examNotFound;
    }
    try {
        summary.studentId = student->getStudentId();
        summary.fullName = student->getFullName();
        summary.rollNo = student->getRollNo();
        summary.className = student->getClassName();
        summary.section = student->getSection();
        summary.examId = exam->getExamId();
        summary.examName = exam->getExamName();
        summary.subjectName = exam->getSubject();
        summary.maximumMarks = exam->getTotalMarks();
        SubjectManager subjectManager("data/subjects.txt");
        subjectManager.loadData();
        Subject* subject = subjectManager.findByName(exam->getSubject());
        if (subject == nullptr) {
            summary.status = StudentReportStatus::subjectNotFound;
            return summary.status;
        }
        summary.subjectName = subject->getSubjectName();
        SavedAttempt saved;
        bool matchingAttemptSeen = false;
        AttemptLookupStatus lookup = loadAttemptForStudent(
            attemptFilePath, studentId, examId, saved, matchingAttemptSeen
        );
        if (lookup == AttemptLookupStatus::notFound) {
            summary.attempted = false;
            summary.status = StudentReportStatus::noAttempt;
            summary.grade.clear();
            return summary.status;
        }
        if (lookup == AttemptLookupStatus::invalid ||
            !validateRecordedMarks(saved, *exam)) {
            summary.attempted = matchingAttemptSeen ||
                                lookup == AttemptLookupStatus::found;
            summary.status = StudentReportStatus::invalidAttempt;
            summary.grade.clear();
            return summary.status;
        }
        ExamAttempt attempt(saved.attemptId, *student, *exam);
        attempt.restoreSavedState(saved.started, saved.submitted,
                                  saved.currentQuestionIndex, saved.answers);
        Evaluation evaluator;
        summary.marksObtained = evaluator.evaluateAttempt(attempt, *exam);
        GradingSystem grading;
        if (!grading.calculatePercentage(summary.marksObtained,
                                         summary.maximumMarks,
                                         summary.percentage) ||
            !grading.calculateGrade(summary.percentage, summary.grade) ||
            !grading.checkPassFail(summary.percentage, summary.passed)) {
            summary.attempted = true;
            summary.status = StudentReportStatus::invalidAttempt;
            summary.grade.clear();
            return summary.status;
        }
        summary.attempted = true;
        summary.status = StudentReportStatus::validAttempt;
        return summary.status;
    } catch (const std::exception&) {
        summary.attempted = true;
        summary.status = StudentReportStatus::invalidAttempt;
        summary.grade.clear();
        return summary.status;
    }
}
bool StudentReport::generateReport(int studentId,
                                   int examId,
                                   std::ostream& out,
                                   const std::string& attemptFilePath) const {
    StudentExamSummary summary;
    StudentReportStatus status = getStudentExamSummary(
        studentId, examId, summary, out, attemptFilePath
    );

    if (status == StudentReportStatus::studentNotFound) {
        out << "Student with ID " << studentId << " does not exist.\n";
        return false;
    }
    if (status == StudentReportStatus::examNotFound) {
        out << "Exam with ID " << examId << " does not exist.\n";
        return false;
    }
    if (summary.studentId == 0 || summary.examId == 0) {
        out << "Could not prepare the report for the selected student and exam.\n";
        return false;
    }
    out << "\n================ STUDENT REPORT ================\n";
    out << "Student ID : " << summary.studentId << "\n";
    out << "Full Name  : " << summary.fullName << "\n";
    out << "Roll No    : " << summary.rollNo << "\n";
    out << "Class      : " << summary.className << "\n";
    out << "Section    : " << summary.section << "\n\n";
    out << "Exam ID    : " << summary.examId << "\n";
    out << "Exam Name  : " << summary.examName << "\n";
    out << "Subject    : " << summary.subjectName << "\n";
    out << "Max Marks  : " << summary.maximumMarks << "\n\n";
    if (status == StudentReportStatus::noAttempt) {
        out << "Attempt status: Not attempted\n";
        out << "Grade         : Not available\n";
        out << "Pass/Fail     : Not available\n";
    } else if (status == StudentReportStatus::subjectNotFound) {
        out << "Attempt status: Result unavailable\n";
        out << "Subject lookup: No matching subject was found\n";
        out << "Grade         : Not available\n";
        out << "Pass/Fail     : Not available\n";
    } else if (status == StudentReportStatus::invalidAttempt) {
        out << "Attempt status: Invalid or unavailable result\n";
        out << "Grade         : Not available\n";
        out << "Pass/Fail     : Not available\n";
    } else {
        out << "Marks Obtained: " << summary.marksObtained << "\n";
        out << "Percentage    : " << summary.percentage << "%\n";
        out << "Grade         : " << summary.grade << "\n";
        out << "Pass/Fail     : " << (summary.passed ? "Pass" : "Fail") << "\n";
    }
    out << "==============================================\n";
    return status == StudentReportStatus::validAttempt ||
           status == StudentReportStatus::noAttempt;
}
