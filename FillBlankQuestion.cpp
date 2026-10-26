#include "FillBlankQuestion.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
using namespace std;
// Constructor
FillBlankQuestion::FillBlankQuestion(int id,const string& text,double marks,const string& answer)
    : Question(id, text, marks),correctAnswer(answer)
{
    if (correctAnswer.find_first_not_of(" \t\n\r") == string::npos)
        throw invalid_argument("Correct answer cannot be empty.");
}
// Display question details
void FillBlankQuestion::display(ostream& out) const
{
    out << "Question ID: " << getQuestionId() << '\n';
    out << "Question: " << getQuestionText() << '\n';
    out << "Maximum Marks: " << getMaxMarks() << '\n';
}
// Accept question details
void FillBlankQuestion::input(istream& in)
{
    string text;
    string answer;
    cout << "Enter question text (use _____ for the blank): ";
    if (!getline(in >> ws, text))
        return;
    if (text.find_first_not_of(" \t\n\r") == string::npos)
    {
        cout << "Question text cannot be empty.\n";
        in.setstate(ios::failbit);
        return;
    }
    cout << "Enter correct answer: ";
    if (!getline(in >> ws, answer))
        return;
    if (answer.find_first_not_of(" \t\n\r") == string::npos)
    {
        cout << "Correct answer cannot be empty.\n";
        in.setstate(ios::failbit);
        return;
    }
    setQuestionText(text);
    correctAnswer = answer;
}
// Save question details to a file
void FillBlankQuestion::saveToFile(ostream& out) const
{
    out << "FILL_BLANK\n";
    out << getQuestionId() << '\n';
    out << getQuestionText() << '\n';
    out << getMaxMarks() << '\n';
    out << correctAnswer << '\n';
}
// Getter
string FillBlankQuestion::getCorrectAnswer() const
{
    return correctAnswer;
}