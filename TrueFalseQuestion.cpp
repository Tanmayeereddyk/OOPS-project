
#include "TrueFalseQuestion.h"
#include <iostream>
#include <stdexcept>
#include <string>
#include <cctype>
#include <fstream>
using namespace std;

// Constructor
TrueFalseQuestion::TrueFalseQuestion(int id,
                                     const string& text,
                                     double marks,
                                     bool answer)
    : Question(id, text, marks),
      correctAnswer(answer)
{
}

// Display the True/False question
void TrueFalseQuestion::display(ostream& out) const
{
    out << "Question ID: " << getQuestionId() << '\n';
    out << "Question: " << getQuestionText() << '\n';
    out << "Maximum Marks: " << getMaxMarks() << '\n';
    out << "Options: True / False" << '\n';
}

// Input the question details
void TrueFalseQuestion::input(istream& in)
{
    string text;
    string answer;

    cout << "Enter question text: ";

    if (!getline(in >> ws, text))
        return;

    // Validate question text
    if (text.find_first_not_of(" \t\n\r") == string::npos)
    {
        cout << "Question text cannot be empty.\n";
        in.setstate(ios::failbit);
        return;
    }

    cout << "Enter correct answer (true/false): ";

    if (!(in >> answer))
        return;

    // Convert answer to lowercase
    for (size_t i = 0; i < answer.length(); ++i)
    {
        answer[i] = static_cast<char>(
            tolower(static_cast<unsigned char>(answer[i]))
        );
    }

    bool newAnswer;

    if (answer == "true")
    {
        newAnswer = true;
    }
    else if (answer == "false")
    {
        newAnswer = false;
    }
    else
    {
        cout << "Invalid answer. Enter only true or false.\n";
        in.setstate(ios::failbit);
        return;
    }

    // Update only after successful validation
    setQuestionText(text);
    correctAnswer = newAnswer;
}

// Save question details to a file
void TrueFalseQuestion::saveToFile(ostream& out) const
{
    out << "TRUE_FALSE\n";
    out << getQuestionId() << '\n';
    out << getQuestionText() << '\n';
    out << getMaxMarks() << '\n';
    out << (correctAnswer ? "true" : "false") << '\n';
}
// Getter
bool TrueFalseQuestion::getCorrectAnswer() const
{
    return correctAnswer;
}
