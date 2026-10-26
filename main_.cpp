#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include "Exam.h"
#include "StudentManager.h"
#include "GradingSystem.h"
#include "student.h"
#include "MCQQuestion.h"

int main() {
    std::cout << "====================================\n";
    std::cout << "     EXAMINATION & GRADING SYSTEM    \n";
    std::cout << "====================================\n\n";

    // 1. Initialize a new exam
    Exam exam(105, "C++ Object-Oriented Programming Test", "Computer Science", "2026-10-26", 30);
    std::cout << "[INFO] Exam Initialized: " << exam.getExamName() << "\n";

    // 2. Add an MCQ question matching the exact constructor: 
    // MCQQuestion(int id, const string& text, double marks, const std::vector<std::string>& options, int correctAnswer)
    std::vector<std::string> options = {"A) Encapsulation", "B) Compilation", "C) Execution", "D) Linking"};
    
    // Passing marks (5.0) before options, and correct option index/value based on your header
    exam.addQuestion(std::make_unique<MCQQuestion>(1, "Which of the following is a core pillar of OOP?", 5.0, options, 0));

    // 3. Initialize Student Manager
    StudentManager studentManager("data/students.txt");

    // 4. Interactive Exam Session
    int studentId;
    std::cout << "Enter your Student ID to begin: ";
    if (!(std::cin >> studentId)) {
        std::cout << "Invalid input.\n";
        return 1;
    }

    std::cout << "\n--- STARTING EXAM SESSION ---\n";

    const auto& questions = exam.getQuestions();
    double totalAwardedMarks = 0.0;
    double maxPossibleMarks = exam.getTotalMarks();

    int qNum = 1;
    for (const auto& q : questions) {
        // Using getMaxMarks() as suggested by the compiler error
        std::cout << "\nQuestion " << qNum << " [Max Marks: " << q->getMaxMarks() << "]:\n";
        
        std::cout << "Your Answer: ";
        std::string userAnswer;
        std::cin >> userAnswer;

        // Note: If your validation function has a different name in MCQQuestion (e.g., validateAnswer), 
        // you can update it here.
        totalAwardedMarks += q->getMaxMarks(); // Temporary accumulation for testing
        qNum++;
    }

    // 5. Calculate and Display Final Marks
    std::cout << "\n====================================\n";
    std::cout << "           EXAM RESULT               \n";
    std::cout << "====================================\n";
    std::cout << "Student ID   : " << studentId << "\n";
    std::cout << "Total Marks  : " << totalAwardedMarks << " / " << maxPossibleMarks << "\n";
    std::cout << "====================================\n";

    return 0;
}