
#ifndef EVALUATION_H
#define EVALUATION_H

class Exam;
class ExamAttempt;

class Evaluation {
public:
    // Evaluate a submitted exam attempt and return total marks.
    double evaluateAttempt(ExamAttempt& attempt, const Exam& exam);
};

#endif
