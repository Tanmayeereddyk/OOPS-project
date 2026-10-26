
#include "examattempt.h"
#include "AttemptStorage.h"
#include "student.h"
#include "Exam.h"
#include "Question.h"
#include "Answer.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
// Constructor
ExamAttempt::ExamAttempt(int attemptId, const Student& student, const Exam& exam)
    : attemptId(attemptId),
      studentId(student.getStudentId()),
      examId(exam.getExamId()),
      exam(&exam),
      answers(),
      currentQuestionIndex(0),
      started(false),
      submitted(false)
{
}

const char* ExamAttempt::defaultAttemptFilePath()
{
    return AttemptStorage::defaultFilePath();
}

void ExamAttempt::restoreSavedState(bool wasStarted,
                                    bool wasSubmitted,
                                    int savedQuestionIndex,
                                    const std::vector<Answer>& savedAnswers)
{
    if (wasSubmitted && !wasStarted) {
        throw std::invalid_argument("A submitted attempt must have been started.");
    }
    if (savedQuestionIndex < 0) {
        throw std::invalid_argument("Saved question index cannot be negative.");
    }

    started = wasStarted;
    submitted = wasSubmitted;
    currentQuestionIndex = savedQuestionIndex;
    answers = savedAnswers;
}

// Start the examination
void ExamAttempt::startExam()
{
    if (submitted) {
        std::cout << "This attempt has already been submitted.\n";
        return;
    }

    if (started) {
        displayCurrentQuestion();
        return;
    }

    if (exam == nullptr || exam->getQuestions().empty()) {
        std::cout << "This exam has no questions.\n";
        return;
    }

    started = true;
    currentQuestionIndex = 0;

    std::cout << "\nExam started successfully!\n";
    std::cout << "Exam ID: " << examId << '\n';
    std::cout << "Total questions: " << exam->getQuestions().size() << "\n\n";
    displayCurrentQuestion();
}

// Display the current question
void ExamAttempt::displayCurrentQuestion()
{
    if (!started) {
        std::cout << "Please start the exam first.\n";
        return;
    }

    const auto& questions = exam->getQuestions();

    if (currentQuestionIndex < 0 || currentQuestionIndex >= static_cast<int>(questions.size())) {
        std::cout << "Invalid question position.\n";
        return;
    }

    std::cout << "\nQuestion " << currentQuestionIndex + 1 << " of " << questions.size() << "\n";

    // Calls the correct derived-class display() function.
    questions[currentQuestionIndex]->display(std::cout);
    std::cout << '\n';
}

// Record the answer for the current question
bool ExamAttempt::acceptAnswer(const std::string& response)
{
    if (!started || submitted) {
        std::cout << "The exam is not active.\n";
        return false;
    }

    const auto& questions = exam->getQuestions();

    if (currentQuestionIndex < 0 || currentQuestionIndex >= static_cast<int>(questions.size())) {
        return false;
    }

    if (response.empty()) {
        std::cout << "Empty answer. Use skipQuestion() if needed.\n";
        return false;
    }

    int questionId = questions[currentQuestionIndex]->getQuestionId();

    // Update an existing answer if the student revisits a question.
    for (auto& answer : answers) {
        if (answer.questionId == questionId) {
            answer.response = response;
            answer.marksAwarded = 0.0;
            answer.evaluated = false;
            std::cout << "Answer updated successfully.\n";
            return true;
        }
    }

    // Otherwise, record a new answer.
    answers.push_back({questionId, response, 0.0, false});

    std::cout << "Answer recorded successfully.\n";
    return true;
}

// Move to the next question
bool ExamAttempt::nextQuestion()
{
    if (!started || submitted) {
        return false;
    }

    const auto& questions = exam->getQuestions();

    if (currentQuestionIndex + 1 >= static_cast<int>(questions.size())) {
        std::cout << "This is the last question.\n";
        return false;
    }

    ++currentQuestionIndex;
    displayCurrentQuestion();
    return true;
}

// Move to the previous question
bool ExamAttempt::previousQuestion()
{
    if (!started || submitted) {
        return false;
    }

    if (currentQuestionIndex <= 0) {
        std::cout << "This is the first question.\n";
        return false;
    }

    --currentQuestionIndex;
    displayCurrentQuestion();

    return true;
}

// Skip the current question without recording a new answer
bool ExamAttempt::skipQuestion()
{
    if (!started || submitted) {
        return false;
    }

    const auto& questions = exam->getQuestions();

    if (currentQuestionIndex + 1 >=
        static_cast<int>(questions.size())) {
        std::cout << "Last question reached. You can submit the exam or answer this question.\n";
        return false;
    }

    ++currentQuestionIndex;
    std::cout << "Question skipped.\n";
    displayCurrentQuestion();

    return true;
}

// Submit and lock the attempt
bool ExamAttempt::submitExam()
{
    if (!started) {
        std::cout << "Start the exam before submitting.\n";
        return false;
    }

    if (submitted) {
        std::cout << "This attempt has already been submitted.\n";
        return false;
    }

    submitted = true;

    std::size_t totalQuestions = exam->getQuestions().size();
    std::size_t answeredQuestions = answers.size();

    std::cout << "\nExam submitted successfully!\n";
    std::cout << "Questions: " << totalQuestions << '\n';
    std::cout << "Answered: " << answeredQuestions << '\n';
    std::cout << "Unanswered: " << totalQuestions - answeredQuestions << '\n';

    return true;
}

// Check submission status
bool ExamAttempt::isSubmitted() const
{
    return submitted;
}

// Getters
int ExamAttempt::getAttemptId() const
{
    return attemptId;
}

int ExamAttempt::getStudentId() const
{
    return studentId;
}

int ExamAttempt::getExamId() const
{
    return examId;
}

const std::vector<Answer>& ExamAttempt::getAnswers() const
{
    return answers;
}

// Save the attempt and its answers to a file
bool ExamAttempt::saveAttempt(const std::string& filename) const
{
    std::ofstream file(filename, std::ios::app);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << filename << " for saving.\n";
        return false;
    }

    file << "ATTEMPT "
         << attemptId << ' '
         << studentId << ' '
         << examId << ' '
         << started << ' '
         << submitted << ' '
         << currentQuestionIndex << '\n';
    file << std::setprecision(std::numeric_limits<double>::max_digits10);
    for (const auto& answer : answers) {
        file << "ANSWER "
             << answer.questionId << ' '
             << std::quoted(answer.response) << ' '
             << answer.marksAwarded << ' '
             << answer.evaluated << '\n';
}

    file << "END_ATTEMPT\n";

    if (!file) {
        std::cerr << "Error while writing attempt data.\n";
        return false;
    }

    std::cout << "Attempt saved successfully to "
              << filename << ".\n";

    return true;
}

bool ExamAttempt::saveAttempt() const
{
    return saveAttempt(defaultAttemptFilePath());
}

std::vector<Answer>&
ExamAttempt::getAnswersForEvaluation()
{
    if (!submitted) {
        throw std::logic_error(
            "Submit the exam before evaluating answers."
        );
    }

    return answers;
}