
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>

#include "Exam.h"
#include "MCQQuestion.h"
#include "TrueFalseQuestion.h"
#include "FillBlankQuestion.h"
#include "SubjectiveQuestion.h"

using namespace std;

int main()
{
    try
    {
        cout << "====================================\n";
        cout << "   EXAMINATION SYSTEM TEST PROGRAM\n";
        cout << "====================================\n";

        // -----------------------------------
        // TEST 1: Create a new examination
        // -----------------------------------
        cout << "\nTEST 1: Creating an exam...\n";

        Exam exam(
            101,
            "OOP Midterm",
            "Object Oriented Programming",
            "2026-11-15",
            90
        );

        cout << "Exam created successfully!\n";

        // -----------------------------------
        // TEST 2: Add an MCQ question
        // -----------------------------------
        cout << "\nTEST 2: Adding an MCQ question...\n";

        vector<string> options;
        options.push_back("Compiler");
        options.push_back("Constructor");
        options.push_back("Destructor");
        options.push_back("Variable");

        exam.addQuestion(
            unique_ptr<Question>(
                new MCQQuestion(
                    1,
                    "Which function initializes an object?",
                    5.0,
                    options,
                    2
                )
            )
        );

        cout << "MCQ question added.\n";

        // -----------------------------------
        // TEST 3: Add a True/False question
        // -----------------------------------
        cout << "\nTEST 3: Adding a True/False question...\n";

        exam.addQuestion(
            unique_ptr<Question>(
                new TrueFalseQuestion(
                    2,
                    "The Earth revolves around the Sun.",
                    2.0,
                    true
                )
            )
        );

        cout << "True/False question added.\n";

        // -----------------------------------
        // TEST 4: Add a Fill-in-the-Blank question
        // -----------------------------------
        cout << "\nTEST 4: Adding a Fill-in-the-Blank question...\n";

        exam.addQuestion(
            unique_ptr<Question>(
                new FillBlankQuestion(
                    3,
                    "The capital of France is _____.",
                    2.0,
                    "Paris"
                )
            )
        );

        cout << "Fill-in-the-Blank question added.\n";

        // -----------------------------------
        // TEST 5: Add a Subjective question
        // -----------------------------------
        cout << "\nTEST 5: Adding a Subjective question...\n";

        exam.addQuestion(
            unique_ptr<Question>(
                new SubjectiveQuestion(
                    4,
                    "Explain encapsulation.",
                    10.0,
                    "Encapsulation combines data and methods "
                    "in a class and controls access to that data."
                )
            )
        );

        cout << "Subjective question added.\n";

        // -----------------------------------
        // TEST 6: Display exam details
        // -----------------------------------
        cout << "\nTEST 6: Displaying exam details...\n";

        exam.displayExam();

        // -----------------------------------
        // TEST 7: Display all questions
        // -----------------------------------
        cout << "\nTEST 7: Displaying all questions...\n";

        exam.displayQuestions();

        // -----------------------------------
        // TEST 8: Test getters
        // -----------------------------------
        cout << "\nTEST 8: Testing getters...\n";

        cout << "Exam ID      : " << exam.getExamId() << '\n';
        cout << "Exam Name    : " << exam.getExamName() << '\n';
        cout << "Subject      : " << exam.getSubject() << '\n';
        cout << "Date         : " << exam.getDate() << '\n';
        cout << "Duration     : " << exam.getDuration()
             << " minutes\n";
        cout << "Total Marks  : " << exam.getTotalMarks() << '\n';

        // -----------------------------------
        // TEST 9: Check question collection
        // -----------------------------------
        cout << "\nTEST 9: Checking question collection...\n";

        cout << "Number of questions: "
             << exam.getQuestions().size() << '\n';

        for (size_t i = 0; i < exam.getQuestions().size(); i++)
        {
            cout << "Question ID: "
                 << exam.getQuestions()[i]->getQuestionId()
                 << ", Marks: "
                 << exam.getQuestions()[i]->getMaxMarks()
                 << '\n';
        }

        // -----------------------------------
        // TEST 10: Remove a question
        // -----------------------------------
        cout << "\nTEST 10: Removing question ID 2...\n";

        if (exam.removeQuestion(2))
        {
            cout << "Question removed successfully.\n";
        }
        else
        {
            cout << "Question not found.\n";
        }

        cout << "Questions remaining: "
             << exam.getQuestions().size() << '\n';

        cout << "Updated total marks: "
             << exam.getTotalMarks() << '\n';

        // -----------------------------------
        // TEST 11: Try removing a nonexistent question
        // -----------------------------------
        cout << "\nTEST 11: Removing nonexistent question ID 999...\n";

        if (!exam.removeQuestion(999))
        {
            cout << "Correctly handled: question not found.\n";
        }

        // -----------------------------------
        // TEST 12: Reload exam from its file
        // -----------------------------------
        cout << "\nTEST 12: Loading exam 101 from file...\n";

        Exam loadedExam(101);

        cout << "Exam loaded successfully!\n";

        loadedExam.displayExam();
        loadedExam.displayQuestions();

        // -----------------------------------
        // TEST 13: Verify loaded data
        // -----------------------------------
        cout << "\nTEST 13: Verifying loaded data...\n";

        cout << "Loaded exam ID     : "
             << loadedExam.getExamId() << '\n';

        cout << "Loaded exam title  : "
             << loadedExam.getExamName() << '\n';

        cout << "Loaded question count: "
             << loadedExam.getQuestions().size() << '\n';

        cout << "Loaded total marks : "
             << loadedExam.getTotalMarks() << '\n';

        cout << "\n====================================\n";
        cout << "       ALL TESTS COMPLETED\n";
        cout << "====================================\n";

        cout << "\nCheck exam_101.txt to verify file persistence.\n";
    }
    catch (const exception& e)
    {
        cerr << "\nERROR: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
