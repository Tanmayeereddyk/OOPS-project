#ifndef EXAMATTEMPT_H
#define EXAMATTEMPT_H
#include <string>
#include <vector>
#include "Answer.h"
class Student;
class Exam;
class ExamAttempt {
private:
    int attemptId;
    int studentId;
    int examId;
    const Exam* exam;
    std::vector<Answer> answers;
    int currentQuestionIndex;
    bool started;
    bool submitted;
public:
    static const char* defaultAttemptFilePath();
    ExamAttempt(int attemptId,
                const Student& student,
                const Exam& exam);
    void restoreSavedState(bool wasStarted,
                           bool wasSubmitted,
                           int savedQuestionIndex,
                           const std::vector<Answer>& savedAnswers);
    void startExam();
    void displayCurrentQuestion();
    bool acceptAnswer(const std::string& response);
    bool nextQuestion();
    bool previousQuestion();
    bool skipQuestion();
    bool submitExam();
    bool isSubmitted() const;
    int getAttemptId() const;
    int getStudentId() const;
    int getExamId() const;
    const std::vector<Answer>& getAnswers() const;
    // Allows the evaluation module to update marks
    // after the attempt has been submitted.
    std::vector<Answer>& getAnswersForEvaluation();
    bool saveAttempt(const std::string& filename) const;
    bool saveAttempt() const;
};
#endif