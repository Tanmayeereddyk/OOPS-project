
#include "Evaluation.h"
#include "examattempt.h"
#include "Answer.h"
#include "Exam.h"
#include "Question.h"
#include "MCQQuestion.h"
#include "TrueFalseQuestion.h"
#include "FillBlankQuestion.h"
#include "SubjectiveQuestion.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <limits>
#include <stdexcept>
namespace {
// Convert text to lowercase and normalize whitespace.
// Useful for checking answers without case sensitivity.
std::string normalizeText(const std::string& text)
{
    std::string result;
    bool pendingSpace = false;
    for (unsigned char ch : text) {
        if (std::isspace(ch)) {
            if (!result.empty()) {
                pendingSpace = true;
            }
        } else {
            if (pendingSpace) {
                result += ' ';
                pendingSpace = false;
            }
            result += static_cast<char>(std::tolower(ch));
        }
    }
    return result;
}
// Convert common true/false responses into Boolean values.
bool parseBoolean(const std::string& response, bool& value)
{
    std::string normalized = normalizeText(response);
    if (normalized == "true" || normalized == "t" ||
        normalized == "yes" || normalized == "y" ||
        normalized == "1") {
        value = true;
        return true;
    }
    if (normalized == "false" || normalized == "f" ||
        normalized == "no" || normalized == "n" ||
        normalized == "0") {
        value = false;
        return true;
    }
    return false;
}
// Obtain marks from the examiner for a subjective answer.
double enterSubjectiveMarks(double maxMarks)
{
    double marks;
    while (true) {
        std::cout << "Enter marks awarded (0 to "
                  << maxMarks << "): ";
        if (std::cin >> marks) {
            if (marks >= 0.0 && marks <= maxMarks) {
                std::cin.ignore(
                    std::numeric_limits<std::streamsize>::max(),
                    '\n'
                );
                return marks;
            }
            std::cout << "Marks must be between 0 and "
                      << maxMarks << ".\n";
        } else {
            if (std::cin.eof()) {
                throw std::runtime_error(
                    "Input ended while entering subjective marks."
                );
            }
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );
            std::cout << "Invalid input. Enter a numeric mark.\n";
        }
    }
}
} // End anonymous namespace
// Evaluate a submitted attempt and return its total marks.
double Evaluation::evaluateAttempt(ExamAttempt& attempt,const Exam& exam)
{
    if (!attempt.isSubmitted()) {
        throw std::logic_error(
            "Cannot evaluate an exam that has not been submitted."
        );
    }
    if (attempt.getExamId() != exam.getExamId()) {
        throw std::invalid_argument(
            "The attempt and exam IDs do not match."
        );
    }
    // Only obtain mutable answers after submission.
    std::vector<Answer>& answers =
        attempt.getAnswersForEvaluation();
    const auto& questions = exam.getQuestions();
    double totalMarks = 0.0;
    for (auto& answer : answers) {
        // Find the question that belongs to this answer.
        const Question* question = nullptr;
        for (const auto& q : questions) {
            if (q->getQuestionId() == answer.questionId) {
                question = q.get();
                break;
            }
        }
        if (question == nullptr) {
            throw std::runtime_error(
                "No matching question found for question ID " +
                std::to_string(answer.questionId)
            );
        }
        // Avoid marking the same answer twice.
        if (answer.evaluated) {
            totalMarks += answer.marksAwarded;
            continue;
        }
        const double maxMarks = question->getMaxMarks();
        const std::string response =
            normalizeText(answer.response);
        double marks = 0.0;
        // 1. Multiple-choice question
        if (const auto* mcq =
                dynamic_cast<const MCQQuestion*>(question)) {
            int correctOption = mcq->getCorrectOption();
            std::vector<std::string> options = mcq->getOptions();
            if (correctOption < 1 ||
                correctOption > static_cast<int>(options.size())) {
                throw std::runtime_error(
                    "Invalid correct option for an MCQ."
                );
            }
            // Accept the correct option as a number,
            // letter (A/B/C/D), or option text.
            std::string expectedLetter(
                1, static_cast<char>('a' + correctOption - 1)
            );
            bool correct =
                response == std::to_string(correctOption) ||
                response == expectedLetter ||
                response == normalizeText(
                    options[correctOption - 1]
                );
            if (correct) {
                marks = maxMarks;
            }
        }
        // 2. True/False question
        else if (const auto* tf =
                     dynamic_cast<const TrueFalseQuestion*>(question)) {
            bool studentAnswer;
            if (parseBoolean(answer.response, studentAnswer) &&
                studentAnswer == tf->getCorrectAnswer()) {
                marks = maxMarks;
            }
        }
        // 3. Fill-in-the-blank question
        else if (const auto* fill =
                     dynamic_cast<const FillBlankQuestion*>(question)) {
            if (response ==
                normalizeText(fill->getCorrectAnswer())) {
                marks = maxMarks;
            }
        }
        // 4. Subjective question
        else if (dynamic_cast<const SubjectiveQuestion*>(question)) {
            std::cout << "\nSubjective question ID: "
                      << answer.questionId << '\n';
            std::cout << "Student's answer: "
                      << answer.response << '\n';
            marks = enterSubjectiveMarks(maxMarks);
        }
        // Reject unexpected question types.
        else {
            throw std::runtime_error(
                "Unsupported question type for question ID " +
                std::to_string(answer.questionId)
            );
        }
        answer.marksAwarded = marks;
        answer.evaluated = true;
        totalMarks += marks;
    }
    std::cout << "\nEvaluation completed.\n";
    std::cout << "Student ID: " << attempt.getStudentId() << '\n';
    std::cout << "Exam ID: " << attempt.getExamId() << '\n';
    std::cout << "Marks obtained: " << totalMarks
              << " / " << exam.getTotalMarks() << '\n';
    return totalMarks;
}