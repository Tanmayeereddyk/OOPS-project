#include "MCQQuestion.h"
#include <iostream>
#include <stdexcept>
#include <fstream>
using namespace std;
// Constructor
MCQQuestion::MCQQuestion(int id,const string& text,double marks,const vector<string>& opts,int correct)
    : Question(id, text, marks),options(opts),correctOption(correct)
{
    // Validate that exactly four options are provided
    if (options.size() != 4)
    {
        throw invalid_argument(
            "An MCQ must have exactly four options.");
    }
    // Validate that no option is empty
    for (size_t i = 0; i < options.size(); ++i)
    {
        if (options[i].find_first_not_of(" \t\n\r")
            == string::npos)
        {
            throw invalid_argument(
                "MCQ options cannot be empty.");
        }
    }
    // Validate the correct option number
    if (correctOption < 1 || correctOption > 4)
    {
        throw invalid_argument(
            "Correct option must be between 1 and 4.");
    }
}
// Display the MCQ
void MCQQuestion::display(ostream& out) const
{
    out << "Question ID: " << getQuestionId() << '\n';
    out << "Question: " << getQuestionText() << '\n';
    out << "Maximum Marks: " << getMaxMarks() << '\n';
    for (size_t i = 0; i < options.size(); ++i)
    {
        out << i + 1 << ". " << options[i] << '\n';
    }
}
// Input the MCQ details
void MCQQuestion::input(istream& in)
{
    string text;
    vector<string> newOptions(4);
    int correct;
    // Read question text
    cout << "Enter question text: ";
    if (!getline(in >> ws, text))
        return;
    if (text.find_first_not_of(" \t\n\r") == string::npos)
    {
        cout << "Question text cannot be empty.\n";
        in.setstate(ios::failbit);
        return;
    }
    // Read exactly four non-empty options
    for (int i = 0; i < 4; ++i)
    {
        cout << "Enter option " << i + 1 << ": ";
        if (!getline(in >> ws, newOptions[i]))
            return;
        if (newOptions[i].find_first_not_of(" \t\n\r")
            == string::npos)
        {
            cout << "Option cannot be empty.\n";
            in.setstate(ios::failbit);
            return;
        }
    }
    // Read correct option number
    cout << "Enter correct option number (1-4): ";
    if (!(in >> correct) || correct < 1 || correct > 4)
    {
        cout << "Invalid choice. Enter a number from 1 to 4.\n";
        in.setstate(ios::failbit);
        return;
    }
    // Update object only after all input is valid
    setQuestionText(text);
    options = newOptions;
    correctOption = correct;
}
void MCQQuestion::saveToFile(ostream& out) const
{
    out << "MCQ\n";
    out << getQuestionId() << '\n';
    out << getQuestionText() << '\n';
    out << getMaxMarks() << '\n';
    for (size_t i = 0; i < options.size(); ++i)
        out << options[i] << '\n';
    out << correctOption << '\n';
}
// Getter functions
vector<string> MCQQuestion::getOptions() const
{
    return options;
}
int MCQQuestion::getCorrectOption() const
{
    return correctOption;
}