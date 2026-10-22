
#include "SubjectiveQuestion.h"
#include <iostream>
#include <stdexcept>

using namespace std;

// Constructor
SubjectiveQuestion::SubjectiveQuestion(int id,
                                       const string& text,
                                       double marks,
                                       const string& answer)
    : Question(id, text, marks),
      modelAnswer(answer)
{
    if (modelAnswer.find_first_not_of(" \t\n\r") == string::npos)
        throw invalid_argument("Model answer cannot be empty.");
}

// Display question details
void SubjectiveQuestion::display(ostream& out) const
{
    out << "Question ID: " << getQuestionId() << '\n';
    out << "Question: " << getQuestionText() << '\n';
    out << "Maximum Marks: " << getMaxMarks() << '\n';
}

// Accept question details
void SubjectiveQuestion::input(istream& in)
{
    string text;
    string answer;

    cout << "Enter subjective question text: ";

    if (!getline(in >> ws, text))
        return;

    if (text.find_first_not_of(" \t\n\r") == string::npos)
    {
        cout << "Question text cannot be empty.\n";
        in.setstate(ios::failbit);
        return;
    }

    cout << "Enter model answer: ";

    if (!getline(in >> ws, answer))
        return;

    if (answer.find_first_not_of(" \t\n\r") == string::npos)
    {
        cout << "Model answer cannot be empty.\n";
        in.setstate(ios::failbit);
        return;
    }

    setQuestionText(text);
    modelAnswer = answer;
}

// Save question details to a file
void SubjectiveQuestion::saveToFile(ostream& out) const
{
    out << "SUBJECTIVE\n";
    out << getQuestionId() << '\n';
    out << getQuestionText() << '\n';
    out << getMaxMarks() << '\n';
    out << modelAnswer << '\n';
}

// Getter
string SubjectiveQuestion::getModelAnswer() const
{
    return modelAnswer;
}
