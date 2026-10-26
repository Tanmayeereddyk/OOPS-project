#include <iostream>
#include <string>
#include <limits>
#include <memory>
#include <iomanip>
#include <cctype>

#include "Exam.h"
#include "StudentManager.h"
#include "student.h"
#include "Question.h"
#include "MCQQuestion.h"
#include "TrueFalseQuestion.h"
#include "FillBlankQuestion.h"
#include "SubjectiveQuestion.h"
#include "Answer.h"
#include "GradingSystem.h"
#include "ExamAttempt.h"
#include "AttemptStorage.h"
#include "StudentReport.h"

using namespace std;

int main()
{
    // --------------------------------------------------
    // 1. CREATE EXAM
    // --------------------------------------------------

    Exam exam(
        105,
        "C++ Object-Oriented Programming Test",
        "Computer Science",
        "2026-10-26",
        30
    );

    // Question 1: MCQ
    vector<string> options = {
        "Encapsulation",
        "Compilation",
        "Execution",
        "Linking"
    };

    exam.addQuestion(unique_ptr<Question>(new MCQQuestion(
        1,
        "Which of the following is a pillar of OOP?",
        5.0,
        options,
        1
    )));

    // Question 2: True/False
    exam.addQuestion(unique_ptr<Question>(new TrueFalseQuestion(
        2,
        "Inheritance allows a class to acquire properties "
        "and methods from another class.",
        5.0,
        true
    )));

    // Question 3: Fill in the blank
    exam.addQuestion(unique_ptr<Question>(new FillBlankQuestion(
        3,
        "A class is a _____ for creating objects.",
        5.0,
        "blueprint"
    )));

    // Question 4: Subjective
    exam.addQuestion(unique_ptr<Question>(new SubjectiveQuestion(
        4,
        "Explain encapsulation in object-oriented programming.",
        10.0,
        "Encapsulation combines data and methods into a class "
        "and restricts direct access to internal data."
    )));

    // --------------------------------------------------
    // 2. LOAD STUDENTS
    // --------------------------------------------------

    StudentManager studentManager("data/students.txt");

    if (!studentManager.loadData())
    {
        cerr << "Error: Could not load student data.\n";
        return 1;
    }

    StudentReport report;

    // --------------------------------------------------
    // 3. MAIN MENU
    // --------------------------------------------------

    int choice;

    do
    {
        cout << "\n========================================\n";
        cout << "       EXAMINATION & GRADING SYSTEM\n";
        cout << "========================================\n";
        cout << "1. Take Examination\n";
        cout << "2. View Student Report / Grade\n";
        cout << "3. Exit\n";
        cout << "========================================\n";
        cout << "Enter your choice: ";

        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a number.\n";
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice)
        {
            // ==========================================
            // OPTION 1: TAKE EXAMINATION
            // ==========================================
            case 1:
            {
                int studentId;

                cout << "\nEnter Student ID: ";

                if (!(cin >> studentId))
                {
                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(), '\n'
                    );
                    cout << "Invalid student ID.\n";
                    break;
                }

                cin.ignore(
                    numeric_limits<streamsize>::max(), '\n'
                );

                Student* student =
                    studentManager.findById(studentId);

                if (student == nullptr)
                {
                    cout << "Student not found.\n";
                    break;
                }

                cout << "\nStudent verified.\n";
                cout << "Exam: "
                     << exam.getExamName() << '\n';

                // Uses the student ID as the attempt ID.
                // This simple setup assumes one attempt per student.
                ExamAttempt attempt(studentId, *student, exam);

                attempt.startExam();

                // Process each question.
                while (true)
                {
                    string response;

                    cout << "\nEnter your answer: ";
                    getline(cin, response);

                    // Retry the current question if the answer
                    // could not be recorded.
                    if (!attempt.acceptAnswer(response))
                    {
                        cout << "Could not save answer. "
                             << "Please try again.\n";
                        continue;
                    }

                    // Move to the next question. If there are no
                    // more questions, finish the exam loop.
                    if (!attempt.nextQuestion())
                    {
                        break;
                    }
                }

                attempt.submitExam();

                // ------------------------------------------
                // Evaluate objective questions.
                // Subjective questions remain pending manual
                // evaluation unless your Evaluation class
                // supports automatic subjective evaluation.
                // ------------------------------------------

                auto& answers = attempt.getAnswersForEvaluation();

                double objectiveMarks = 0.0;
                double pendingMarks = 0.0;

                for (auto& answer : answers)
                {
                    switch (answer.questionId)
                    {
                        case 1:
                        {
                            // MCQ: correct option is option 1.
                            if (answer.response == "1")
                            {
                                answer.marksAwarded = 5.0;
                            }
                            else
                            {
                                answer.marksAwarded = 0.0;
                            }

                            answer.evaluated = true;
                            objectiveMarks += answer.marksAwarded;
                            break;
                        }

                        case 2:
                        {
                            // True/False: accepted answers are
                            // true/false, t/f, or 1/0.
                            string response = answer.response;

                            for (char& ch : response)
                            {
                                ch = static_cast<char>(
                                    tolower(
                                        static_cast<unsigned char>(ch)
                                    )
                                );
                            }

                            if (response == "true" ||
                                response == "t" ||
                                response == "1")
                            {
                                answer.marksAwarded = 5.0;
                            }
                            else
                            {
                                answer.marksAwarded = 0.0;
                            }

                            answer.evaluated = true;
                            objectiveMarks += answer.marksAwarded;
                            break;
                        }

                        case 3:
                        {
                            // Fill-in-the-blank.
                            string response = answer.response;

                            for (char& ch : response)
                            {
                                ch = static_cast<char>(
                                    tolower(
                                        static_cast<unsigned char>(ch)
                                    )
                                );
                            }

                            // Trim leading and trailing spaces.
                            size_t first = response.find_first_not_of(" \t\r\n");
                            size_t last = response.find_last_not_of(" \t\r\n");

                            if (first == string::npos)
                            {
                                response.clear();
                            }
                            else
                            {
                                response = response.substr(
                                    first, last - first + 1
                                );
                            }

                            if (response == "blueprint")
                            {
                                answer.marksAwarded = 5.0;
                            }
                            else
                            {
                                answer.marksAwarded = 0.0;
                            }

                            answer.evaluated = true;
                            objectiveMarks += answer.marksAwarded;
                            break;
                        }

                        case 4:
                        {
                            // Subjective answer needs manual marking.
                            answer.marksAwarded = 0.0;
                            answer.evaluated = false;
                            pendingMarks += 10.0;
                            break;
                        }

                        default:
                        {
                            answer.marksAwarded = 0.0;
                            answer.evaluated = false;
                            break;
                        }
                    }
                }

                // Save the attempt to attempts.txt.
                if (attempt.saveAttempt())
                {
                    cout << "\nAttempt saved successfully.\n";
                }
                else
                {
                    cout << "\nError saving attempt.\n";
                }

                cout << fixed << setprecision(2);
                cout << "\n========== EXAM SUMMARY ==========\n";
                cout << "Objective marks: " << objectiveMarks << '\n';
                cout << "Subjective marks pending: "
                     << pendingMarks << '\n';
                cout << "Maximum marks: " << exam.getTotalMarks() << '\n';
                cout << "==================================\n";

                cout << "Note: The final report may remain invalid "
                     << "until all answers are evaluated.\n";

                break;
            }

            // ==========================================
            // OPTION 2: VIEW STUDENT REPORT
            // ==========================================
            case 2:
            {
                int studentId;
                int examId;

                cout << "\nEnter Student ID: ";

                if (!(cin >> studentId))
                {
                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(), '\n'
                    );
                    cout << "Invalid student ID.\n";
                    break;
                }

                cout << "Enter Exam ID: ";

                if (!(cin >> examId))
                {
                    cin.clear();
                    cin.ignore(
                        numeric_limits<streamsize>::max(), '\n'
                    );
                    cout << "Invalid exam ID.\n";
                    break;
                }

                cin.ignore(
                    numeric_limits<streamsize>::max(), '\n'
                );

                StudentExamSummary summary;

                StudentReportStatus status =
                    StudentReport::getStudentExamSummary(
                        studentId,
                        examId,
                        summary,
                        cout,
                        AttemptStorage::defaultFilePath()
                    );

                if (status == StudentReportStatus::validAttempt)
                {
                    cout << fixed << setprecision(2);

                    cout << "\n========== STUDENT REPORT ==========\n";
                    cout << "Student ID: " << summary.studentId << '\n';
                    cout << "Student Name: " << summary.fullName << '\n';
                    cout << "Roll Number: " << summary.rollNo << '\n';
                    cout << "Class: " << summary.className << '\n';
                    cout << "Section: " << summary.section << '\n';

                    cout << "\nExam ID: " << summary.examId << '\n';
                    cout << "Exam Name: " << summary.examName << '\n';
                    cout << "Subject: " << summary.subjectName << '\n';

                    cout << "\nMaximum Marks: "
                         << summary.maximumMarks << '\n';
                    cout << "Marks Obtained: "
                         << summary.marksObtained << '\n';
                    cout << "Percentage: "
                         << summary.percentage << "%\n";
                    cout << "Grade: " << summary.grade << '\n';
                    cout << "Result: "
                         << (summary.passed ? "PASS" : "FAIL")
                         << '\n';

                    cout << "====================================\n";
                }
                else
                {
                    cout << "\nCould not generate a valid report.\n";

                    switch (status)
                    {
                        case StudentReportStatus::noAttempt:
                            cout << "Reason: No saved attempt found.\n";
                            break;

                        case StudentReportStatus::invalidAttempt:
                            cout << "Reason: The attempt is incomplete "
                                 << "or contains answers that have not "
                                 << "been evaluated.\n";
                            break;

                        case StudentReportStatus::studentNotFound:
                            cout << "Reason: Student not found.\n";
                            break;

                        case StudentReportStatus::examNotFound:
                            cout << "Reason: Exam not found.\n";
                            break;

                        case StudentReportStatus::subjectNotFound:
                            cout << "Reason: Subject not found.\n";
                            break;

                        case StudentReportStatus::validAttempt:
                            break;
                    }
                }

                break;
            }

            // ==========================================
            // OPTION 3: EXIT
            // ==========================================
            case 3:
            {
                cout << "\nExiting Examination & Grading System.\n";
                break;
            }

            default:
            {
                cout << "Invalid choice. Please select 1, 2, or 3.\n";
                break;
            }
        }

    } while (choice != 3);

    return 0;
}
