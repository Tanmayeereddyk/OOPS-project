#include "Question.h"
#include <stdexcept>
#include <iostream>
using namespace std;
// Constructor
Question::Question(int id, const std::string& text, double marks) : questionId(id), questionText(text), maxMarks(marks)
{
    if (id <= 0)
    {
        throw invalid_argument("Question ID must be positive.");
    }
    if (marks < 0)
    {
        throw invalid_argument("Maximum marks cannot be negative.");
    }
}
// Virtual destructor
Question::~Question()
{}
// Getter functions
int Question::getQuestionId() const
{
    return questionId;
}
string Question::getQuestionText() const
{
    return questionText;
}
double Question::getMaxMarks() const
{
    return maxMarks;
}
// Setter functions
void Question::setQuestionId(int id)
{
    if (id <= 0)
    {
        throw invalid_argument("Question ID must be positive.");
    }
    questionId = id;
}
void Question::setQuestionText(const string& text)
{
    questionText = text;
}
void Question::setMaxMarks(double marks)
{
    if (marks < 0)
    {
        throw invalid_argument("Maximum marks cannot be negative.");
    }
    maxMarks = marks;
}
// Overloaded output operator
ostream& operator<<(ostream& out, const Question& q)
{
    q.display(out);
    return out;
}
// Overloaded input operator
istream& operator>>(istream& in, Question& q)
{
    q.input(in);
    return in;
}