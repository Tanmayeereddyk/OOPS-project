
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <iomanip>

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
#include "StudentReport.h"

int main()
{
    std::cout << "====================================\n";
    std::cout << "     EXAMINATION & GRADING SYSTEM    \n";
    std::cout << "====================================\n\n";

    // 1. Create the examination
    Exam exam(
        105,
        "C++ Object-Oriented Programming Test",
        "Computer Science",
        "2026-10-26",
        30
    );

    std::cout << "[INFO] Exam Initialized: "
              << exam.getExamName() << "\n";

    // 2. Add an MCQ question
    // Options are displayed as 1, 2, 3, 4 automatically.
    std::vector<std::string> options = {
        "Encapsulation",
        "Compilation",
        "Execution",
        "Linking"
    };

    exam.addQuestion(
        std::make_unique<MCQQuestion>(
            1,
            "Which of the following is a core pillar of OOP?",
            5.0,
            options,
            1
        )
    );

    // 3. Add a True/False question
    exam.addQuestion(
        std::make_unique<TrueFalseQuestion>(
            2,
            "Inheritance allows a class to acquire features "
            "from another class.",
            5.0,
            true
        )
    );

    // 4. Add a Fill-in-the-Blank question
    exam.addQuestion(
        std::make_unique<FillBlankQuestion>(
            3,
            "A class is a _____ for creating objects.",
            5.0,
            "blueprint"
        )
    );

    // 5. Add a Subjective question
    exam.addQuestion(
        std::make_unique<SubjectiveQuestion>(
            4,
            "Explain encapsulation in object-oriented "
            "programming.",
            10.0,
            "Encapsulation combines data and methods in a "
            "class and controls access to an object's data."
        )
    );

    // 6. Initialize Student Manager
    StudentManager studentManager("data/students.txt");

    int studentId;

    std::cout << "\nEnter your Student ID to begin: ";

    if (!(std::cin >> studentId))
    {
        std::cout << "Invalid Student ID.\n";
        return 1;
    }

    std::cout << "\n--- STARTING EXAM SESSION ---\n";

    const auto& questions = exam.getQuestions();

    double totalAwardedMarks = 0.0;
    double pendingSubjectiveMarks = 0.0;
    double maxPossibleMarks = exam.getTotalMarks();

    int qNum = 1;

    std::vector<Answer> studentAnswers;

    // Demo answer key for the three automatically graded questions.
    // These IDs and keys must match the questions created above.
    const std::map<int, std::string> answerKey = {
        {1, "1"},          // MCQ: Encapsulation
        {2, "true"},       // True/False
        {3, "blueprint"}   // Fill in the blank
    };

    // Normalize answers for case-insensitive text comparison.
    auto normalize = [](std::string value) {
        std::transform(
            value.begin(),
            value.end(),
            value.begin(),
            [](unsigned char ch) {
                return static_cast<char>(std::tolower(ch));
            }
        );

        const std::string whitespace = " \t\n\r";

        std::size_t first = value.find_first_not_of(whitespace);

        if (first == std::string::npos)
            return std::string("");

        std::size_t last = value.find_last_not_of(whitespace);

        return value.substr(first, last - first + 1);
    };

    // 7. Display questions and collect answers
    for (const auto& q : questions)
    {
        std::cout << "\n========== Question "
                  << qNum << " ==========\n";

        q->display(std::cout);

        std::string userResponse;

        std::cout << "\nYour Answer: ";
        std::getline(std::cin >> std::ws, userResponse);

        const SubjectiveQuestion* subjective =
            dynamic_cast<const SubjectiveQuestion*>(q.get());

        Answer answer(
            q->getQuestionId(),
            userResponse,
            0.0,
            false
        );

        if (subjective != nullptr)
        {
            // Subjective responses need teacher grading.
            pendingSubjectiveMarks += q->getMaxMarks();

            std::cout
                << "Answer recorded for manual grading.\n";

            answer.evaluated = false;
        }
        else
        {
            std::string normalizedResponse = normalize(userResponse);

            // Accept T/F as alternatives to True/False.
            if (normalizedResponse == "t")
                normalizedResponse = "true";
            else if (normalizedResponse == "f")
                normalizedResponse = "false";

            const MCQQuestion* mcq =
                dynamic_cast<const MCQQuestion*>(q.get());

            // Validate MCQ input.
            if (mcq != nullptr &&
                (normalizedResponse < "1" ||
                 normalizedResponse > "4" ||
                 normalizedResponse.size() != 1))
            {
                std::cout
                    << "Invalid option. No marks awarded.\n";
            }
            else
            {
                auto key = answerKey.find(q->getQuestionId());

                if (key == answerKey.end())
                {
                    std::cout
                        << "No answer key configured for this "
                        << "question. It needs evaluation.\n";
                }
                else
                {
                    answer.evaluated = true;

                    if (normalizedResponse == key->second)
                    {
                        answer.marksAwarded = q->getMaxMarks();

                        std::cout << "Correct answer!\n";
                    }
                    else
                    {
                        answer.marksAwarded = 0.0;

                        std::cout << "Incorrect answer.\n";
                    }
                }
            }
        }

        totalAwardedMarks += answer.marksAwarded;
        studentAnswers.push_back(answer);

        qNum++;
    }
    
    // Generate the grade and result summary
    GradingSystem grading;

    double percentage = 0.0;
    std::string grade;
    bool passed = false;

    double gradedMaximum =
        maxPossibleMarks - pendingSubjectiveMarks;

    std::cout << std::fixed << std::setprecision(2);

    std::cout << "\n========================================\n";
    std::cout << "           EXAM RESULT REPORT           \n";
    std::cout << "========================================\n";

    std::cout << "Student ID   : " << studentId << '\n';
    std::cout << "Exam ID      : " << exam.getExamId() << '\n';
    std::cout << "Exam Name    : " << exam.getExamName() << '\n';
    std::cout << "Subject      : " << exam.getSubject() << '\n';
    std::cout << "Maximum Marks: " << maxPossibleMarks << '\n';
    std::cout << "Marks Earned : " << totalAwardedMarks << '\n';

    if (pendingSubjectiveMarks > 0.0)
    {
        std::cout << "Subjective marks pending: "
                  << pendingSubjectiveMarks << '\n';
        std::cout << "Status       : Pending teacher evaluation\n";

        if (gradedMaximum > 0.0 &&
            grading.calculatePercentage(
                totalAwardedMarks, gradedMaximum, percentage) &&
            grading.calculateGrade(percentage, grade))
        {
            std::cout << "Objective %  : " << percentage << "%\n";
            std::cout << "Provisional Grade: " << grade << '\n';
        }
        else
        {
            std::cout << "Grade        : Pending evaluation\n";
        }
    }
    else if (
        grading.calculatePercentage(
            totalAwardedMarks, maxPossibleMarks, percentage) &&
        grading.calculateGrade(percentage, grade) &&
        grading.checkPassFail(percentage, passed))
    {
        std::cout << "Percentage   : " << percentage << "%\n";
        std::cout << "Grade        : " << grade << '\n';
        std::cout << "Result       : "
                  << (passed ? "PASS" : "FAIL") << '\n';
    }
    else
    {
        std::cout << "Grade        : Could not calculate result\n";
    }

    std::cout << "========================================\n";

    // 8. Display results after all questions
    std::cout << "\n====================================\n";
    std::cout << "             EXAM RESULT             \n";
    std::cout << "====================================\n";

    std::cout << "Student ID             : " << studentId << '\n';
    std::cout << "Marks awarded so far   : "
              << totalAwardedMarks << " / "
              << maxPossibleMarks << '\n';

    std::cout << "Subjective marks pending: "
              << pendingSubjectiveMarks << '\n';

    std::cout << "====================================\n";

    return 0;
}
