#include <algorithm>
#include <cmath>
#include <dirent.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "AttemptStorage.h"
#include "ClassReport.h"
#include "Evaluation.h"
#include "Exam.h"
#include "examattempt.h"
#include "FillBlankQuestion.h"
#include "GradingSystem.h"
#include "MCQQuestion.h"
#include "Question.h"
#include "StudentReport.h"
#include "TrueFalseQuestion.h"
#include "SubjectiveQuestion.h"
#include "studentmanager.h"
#include "subjectmanager.h"
#include "validation.h"

namespace {

const std::string STUDENT_FILE = "data/students.txt";
const std::string SUBJECT_FILE = "data/subjects.txt";
const std::string EXAM_DIRECTORY = "exams";

double readNonNegativeDouble(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            throw std::runtime_error("Input ended while reading a number.");
        }

        std::istringstream input(line);
        double value = 0.0;
        if ((input >> value) && (input >> std::ws).eof() &&
            std::isfinite(value) && value >= 0.0) {
            return value;
        }
        std::cout << "Enter a finite number greater than or equal to zero.\n";
    }
}

std::vector<int> getExamIds() {
    std::vector<int> examIds;
    DIR* directory = opendir(EXAM_DIRECTORY.c_str());
    if (directory == nullptr) {
        return examIds;
    }

    const std::string prefix = "exam_";
    const std::string suffix = ".txt";
    struct dirent* entry = nullptr;
    while ((entry = readdir(directory)) != nullptr) {
        const std::string filename(entry->d_name);
        if (filename.size() <= prefix.size() + suffix.size() ||
            filename.compare(0, prefix.size(), prefix) != 0 ||
            filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) != 0) {
            continue;
        }

        const std::string idText = filename.substr(
            prefix.size(), filename.size() - prefix.size() - suffix.size()
        );
        std::istringstream parser(idText);
        int examId = 0;
        if ((parser >> examId) && (parser >> std::ws).eof() && examId > 0) {
            examIds.push_back(examId);
        }
    }
    closedir(directory);
    std::sort(examIds.begin(), examIds.end());
    return examIds;
}

void showExams() {
    const std::vector<int> examIds = getExamIds();
    if (examIds.empty()) {
        std::cout << "No exam files were found in " << EXAM_DIRECTORY << ".\n";
        return;
    }

    std::cout << "\n========== AVAILABLE EXAMS ==========\n";
    for (int id : examIds) {
        try {
            Exam exam(id);
            std::cout << "ID: " << exam.getExamId()
                      << " | Title: " << exam.getExamName()
                      << " | Subject: " << exam.getSubject()
                      << " | Date: " << exam.getDate()
                      << " | Duration: " << exam.getDuration() << " minutes"
                      << " | Marks: " << exam.getTotalMarks() << '\n';
        } catch (const std::exception& error) {
            std::cout << "Exam " << id << " could not be loaded: "
                      << error.what() << '\n';
        }
    }
    std::cout << "=====================================\n";
}

bool hasSavedAttempt(int studentId, int examId, bool matchExam) {
    std::ifstream input(AttemptStorage::defaultFilePath());
    if (!input.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(input, line)) {
        std::istringstream record(line);
        std::string marker;
        int attemptId = 0;
        int savedStudentId = 0;
        int savedExamId = 0;
        int started = 0;
        int submitted = 0;
        int questionIndex = 0;
        if (!(record >> marker) || marker != "ATTEMPT") {
            continue;
        }
        if (!(record >> attemptId >> savedStudentId >> savedExamId
                     >> started >> submitted >> questionIndex)) {
            // Treat an unreadable attempt header conservatively for deletion.
            return true;
        }
        if (savedStudentId == studentId &&
            (!matchExam || savedExamId == examId)) {
            return true;
        }
    }
    return false;
}

bool subjectHasExam(int subjectId, SubjectManager& subjects) {
    Subject* subject = subjects.findById(subjectId);
    if (subject == nullptr) {
        return true;
    }

    const std::vector<int> examIds = getExamIds();
    for (int id : examIds) {
        try {
            Exam exam(id);
            if (Validation::sameIgnoreCase(exam.getSubject(), subject->getSubjectName())) {
                return true;
            }
        } catch (const std::exception&) {
            // A malformed exam cannot be safely checked for a subject link.
            return true;
        }
    }
    return false;
}

void manageStudents(StudentManager& students) {
    students.showMenu([](int studentId) {
        return hasSavedAttempt(studentId, 0, false);
    });
    if (!students.saveData()) {
        std::cout << "Student records could not be saved.\n";
    }
}

void manageSubjects(SubjectManager& subjects) {
    subjects.showMenu([&subjects](int subjectId) {
        return subjectHasExam(subjectId, subjects);
    });
    if (!subjects.saveData()) {
        std::cout << "Subject records could not be saved.\n";
    }
}

std::unique_ptr<Question> createQuestion() {
    const int questionId = Validation::readNumber("Question ID: ", 1, 999999);
    const std::string text = Validation::readText("Question text: ");
    const double marks = readNonNegativeDouble("Maximum marks: ");

    std::cout << "Question type: 1. MCQ  2. True/False  "
                 "3. Fill in the blank  4. Subjective\n";
    const int type = Validation::readNumber("Choose type: ", 1, 4);

    if (type == 1) {
        std::vector<std::string> options;
        for (int i = 1; i <= 4; ++i) {
            options.push_back(Validation::readText(
                "Option " + std::to_string(i) + ": "
            ));
        }
        const int correct = Validation::readNumber("Correct option (1-4): ", 1, 4);
        return std::unique_ptr<Question>(
            new MCQQuestion(questionId, text, marks, options, correct)
        );
    }
    if (type == 2) {
        const int correct = Validation::readNumber(
            "Correct answer (1. True, 2. False): ", 1, 2
        );
        return std::unique_ptr<Question>(
            new TrueFalseQuestion(questionId, text, marks, correct == 1)
        );
    }
    if (type == 3) {
        const std::string answer = Validation::readText("Correct answer: ");
        return std::unique_ptr<Question>(
            new FillBlankQuestion(questionId, text, marks, answer)
        );
    }

    const std::string modelAnswer = Validation::readText("Model answer: ");
    return std::unique_ptr<Question>(
        new SubjectiveQuestion(questionId, text, marks, modelAnswer)
    );
}

void createExam(SubjectManager& subjects) {
    if (subjects.countSubjects() == 0) {
        std::cout << "Create a subject before creating an exam.\n";
        return;
    }

    subjects.showAllSubjects();
    const int subjectId = Validation::readNumber("Subject ID: ", 1, 999999);
    Subject* subject = subjects.findById(subjectId);
    if (subject == nullptr) {
        std::cout << "Subject not found.\n";
        return;
    }

    const int examId = Validation::readNumber("New exam ID: ", 1, 999999);
    const std::string title = Validation::readText("Exam title: ");
    const std::string date = Validation::readText("Exam date: ");
    const int duration = Validation::readNumber("Duration in minutes: ", 1, 999999);

    try {
        Exam exam(examId, title, subject->getSubjectName(), date, duration);
        std::cout << "Exam created. Add questions from the exam menu.\n";
    } catch (const std::exception& error) {
        std::cout << "Exam could not be created: " << error.what() << '\n';
    }
}

void manageExams(SubjectManager& subjects) {
    int choice = -1;
    while (choice != 0) {
        std::cout << "\n========== EXAM MANAGEMENT ==========\n"
                     "1. List exams\n"
                     "2. Create exam\n"
                     "3. View exam and question paper\n"
                     "4. Add question to exam\n"
                     "5. Remove question from exam\n"
                     "0. Back\n";
        choice = Validation::readNumber("Choice: ", 0, 5);

        if (choice == 1) {
            showExams();
        } else if (choice == 2) {
            createExam(subjects);
        } else if (choice == 3) {
            const int id = Validation::readNumber("Exam ID: ", 1, 999999);
            try {
                Exam exam(id);
                exam.displayExam();
                exam.displayQuestions();
            } catch (const std::exception& error) {
                std::cout << "Exam could not be loaded: " << error.what() << '\n';
            }
        } else if (choice == 4) {
            const int id = Validation::readNumber("Exam ID: ", 1, 999999);
            try {
                Exam exam(id);
                std::unique_ptr<Question> question = createQuestion();
                exam.addQuestion(std::move(question));
                std::cout << "Question added and exam paper saved.\n";
            } catch (const std::exception& error) {
                std::cout << "Question could not be added: " << error.what() << '\n';
            }
        } else if (choice == 5) {
            const int id = Validation::readNumber("Exam ID: ", 1, 999999);
            const int questionId = Validation::readNumber("Question ID: ", 1, 999999);
            try {
                Exam exam(id);
                if (exam.removeQuestion(questionId)) {
                    std::cout << "Question removed and exam paper saved.\n";
                } else {
                    std::cout << "Question not found in that exam.\n";
                }
            } catch (const std::exception& error) {
                std::cout << "Exam could not be updated: " << error.what() << '\n';
            }
        }
    }
}

void takeExam(StudentManager& students) {
    showExams();
    const int examId = Validation::readNumber("Exam ID: ", 1, 999999);
    const int studentId = Validation::readNumber("Student ID: ", 1, 999999);
    Student* student = students.findById(studentId);
    if (student == nullptr) {
        std::cout << "Student not found.\n";
        return;
    }
    if (hasSavedAttempt(studentId, examId, true)) {
        std::cout << "A saved attempt already exists for this student and exam.\n";
        return;
    }

    try {
        Exam exam(examId);
        if (exam.getQuestions().empty()) {
            std::cout << "This exam has no questions.\n";
            return;
        }

        ExamAttempt attempt(studentId, *student, exam);
        attempt.startExam();
        bool finished = false;
        while (!finished) {
            std::cout << "\n1. Answer current question\n"
                         "2. Next question\n"
                         "3. Previous question\n"
                         "4. Skip current question\n"
                         "5. Submit exam\n";
            const int action = Validation::readNumber("Action: ", 1, 5);
            if (action == 1) {
                const std::string response = Validation::readText("Your answer: ");
                attempt.acceptAnswer(response);
            } else if (action == 2) {
                attempt.nextQuestion();
            } else if (action == 3) {
                attempt.previousQuestion();
            } else if (action == 4) {
                attempt.skipQuestion();
            } else if (attempt.getAnswers().empty()) {
                std::cout << "Record at least one answer before submitting.\n";
            } else if (attempt.submitExam()) {
                Evaluation evaluator;
                const double marks = evaluator.evaluateAttempt(attempt, exam);
                GradingSystem grading;
                double percentage = 0.0;
                std::string grade;
                bool passed = false;
                if (grading.calculatePercentage(marks, exam.getTotalMarks(), percentage) &&
                    grading.calculateGrade(percentage, grade) &&
                    grading.checkPassFail(percentage, passed)) {
                    std::cout << std::fixed << std::setprecision(2)
                              << "Percentage: " << percentage << "%\n"
                              << "Grade: " << grade << "\n"
                              << "Result: " << (passed ? "PASS" : "FAIL") << '\n';
                } else {
                    std::cout << "A result could not be calculated for this exam.\n";
                }

                if (!attempt.saveAttempt()) {
                    std::cout << "The evaluated attempt could not be saved.\n";
                }
                finished = true;
            }
        }
    } catch (const std::exception& error) {
        std::cout << "The exam attempt could not be completed: "
                  << error.what() << '\n';
    }
}

void showStudentReport() {
    const int studentId = Validation::readNumber("Student ID: ", 1, 999999);
    const int examId = Validation::readNumber("Exam ID: ", 1, 999999);
    StudentReport report;
    report.generateReport(studentId, examId);
}

void showClassReport() {
    const int examId = Validation::readNumber("Exam ID: ", 1, 999999);
    const std::string className = Validation::readText("Class: ");
    const std::string section = Validation::readText("Section: ");
    ClassReport report;
    report.generateReport(examId, className, section);
}

} // namespace

int main() {
    StudentManager students(STUDENT_FILE);
    SubjectManager subjects(SUBJECT_FILE);
    if (!students.loadData()) {
        std::cerr << "Student records could not be loaded.\n";
        return 1;
    }
    if (!subjects.loadData()) {
        std::cerr << "Subject records could not be loaded.\n";
        return 1;
    }

    int choice = -1;
    try {
        while (choice != 0) {
            std::cout << "\n==========================================\n"
                         " STUDENT ASSESSMENT AND GRADING SYSTEM\n"
                         "==========================================\n"
                         "1. Student management\n"
                         "2. Subject management\n"
                         "3. Exam management\n"
                         "4. Take an exam\n"
                         "5. Student report\n"
                         "6. Class report and statistics\n"
                         "0. Exit\n";
            choice = Validation::readNumber("Choice: ", 0, 6);

            switch (choice) {
                case 1: manageStudents(students); break;
                case 2: manageSubjects(subjects); break;
                case 3: manageExams(subjects); break;
                case 4: takeExam(students); break;
                case 5: showStudentReport(); break;
                case 6: showClassReport(); break;
                case 0: break;
            }
        }
    } catch (const std::exception& error) {
        std::cerr << "Application stopped: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
