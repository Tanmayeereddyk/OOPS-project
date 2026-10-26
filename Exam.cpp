
#include "Exam.h"

#include "MCQQuestion.h"
#include "TrueFalseQuestion.h"
#include "FillBlankQuestion.h"
#include "SubjectiveQuestion.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <cctype>
#include <dirent.h>
#include <cerrno>
using namespace std;


// Constructor: creates a new examination
Exam::Exam(int id,
           const string& examTitle,
           const string& examSubject,
           const string& examDate,
           int duration)
    : examId(id),
      title(examTitle),
      subject(examSubject),
      date(examDate),
      time(duration),
      subjectId(Subject::findSubjectIdByName(examSubject)),
      maxMarks(0.0),
      questions()
{
    if (examId <= 0)
        throw invalid_argument("Exam ID must be greater than zero.");

    if (title.empty())
        throw invalid_argument("Exam title cannot be empty.");

    if (subject.empty())
        throw invalid_argument("Exam subject cannot be empty.");

    if (date.empty())
        throw invalid_argument("Exam date cannot be empty.");

    if (time <= 0)
        throw invalid_argument("Exam duration must be greater than zero.");

    createExamFile();
}


// Constructor: loads an existing examination by ID
Exam::Exam(int id)
    : examId(id),
      title(""),
      subject(""),
      date(""),
      time(0),
      subjectId(-1),
      maxMarks(0.0),
      questions()
{
    if (examId <= 0)
        throw invalid_argument("Exam ID must be greater than zero.");

    loadExamFromFile();
}


// Generates the filename for this examination
string Exam::getFileName() const
{
    return "exams/exam_" + to_string(examId) + ".txt";
}


// Creates a new exam file without overwriting an existing one
void Exam::createExamFile() const
{
    ifstream existingFile(getFileName().c_str());

    if (existingFile.good())
    {
        existingFile.close();
        throw runtime_error(
            "An exam with this ID already exists."
        );
    }

    existingFile.close();

    saveExamToFile();
}


// Saves exam details and all questions to the file
void Exam::saveExamToFile() const
{
    ofstream out(getFileName().c_str(), ios::out | ios::trunc);

    if (!out.is_open())
        throw runtime_error("Could not open exam file for saving.");

    out << setprecision(numeric_limits<double>::max_digits10);
    out << "EXAM\n";
    out << examId << '\n';
    out << title << '\n';
    out << subject << '\n';
    out << date << '\n';
    out << time << '\n';
    out << "QUESTIONS\n";

    for (size_t i = 0; i < questions.size(); i++)
    {
        questions[i]->saveToFile(out);
    }

    out.close();

    if (!out)
        throw runtime_error("An error occurred while saving the exam.");
}


// Loads exam details and questions from the file
void Exam::loadExamFromFile()
{
    ifstream in(getFileName().c_str());

    if (!in.is_open())
        throw runtime_error("Exam file not found.");

    string marker;
    int savedExamId;

    // Read file header
    if (!getline(in, marker) || marker != "EXAM")
        throw runtime_error("Invalid exam file format.");

    if (!(in >> savedExamId))
        throw runtime_error("Could not read exam ID.");

    in.ignore(numeric_limits<streamsize>::max(), '\n');

    if (savedExamId != examId)
        throw runtime_error("Exam ID does not match the filename.");

    // Read exam details
    if (!getline(in, title) ||
        !getline(in, subject) ||
        !getline(in, date))
    {
        throw runtime_error("Incomplete exam details in file.");
    }

    if (!(in >> time))
        throw runtime_error("Could not read exam duration.");

    in.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!getline(in, marker) || marker != "QUESTIONS")
        throw runtime_error("Missing QUESTIONS marker in exam file.");

    maxMarks = 0.0;

    // Read question records until the end of the file
    string type;

    while (getline(in, type))
    {
        if (type.empty())
            continue;

        int questionId;
        double marks;
        string questionText;

        if (!(in >> questionId))
            throw runtime_error("Invalid question ID in exam file.");

        in.ignore(numeric_limits<streamsize>::max(), '\n');

        if (!getline(in, questionText))
            throw runtime_error("Could not read question text.");

        if (!(in >> marks))
            throw runtime_error("Could not read question marks.");

        in.ignore(numeric_limits<streamsize>::max(), '\n');

        // Check for duplicate question IDs
        for (size_t i = 0; i < questions.size(); i++)
        {
            if (questions[i]->getQuestionId() == questionId)
                throw runtime_error("Duplicate question ID in exam file.");
        }

        // Load a multiple-choice question
        if (type == "MCQ")
        {
            vector<string> options(4);

            for (int i = 0; i < 4; i++)
            {
                if (!getline(in, options[i]))
                    throw runtime_error("Could not read MCQ options.");
            }

            int correctOption;

            if (!(in >> correctOption))
                throw runtime_error("Could not read MCQ correct option.");

            in.ignore(numeric_limits<streamsize>::max(), '\n');

            questions.push_back(
                unique_ptr<Question>(
                    new MCQQuestion(
                        questionId,
                        questionText,
                        marks,
                        options,
                        correctOption
                    )
                )
            );
        }

        // Load a true/false question
        else if (type == "TRUE_FALSE")
        {
            string answer;

            if (!getline(in, answer))
                throw runtime_error("Could not read true/false answer.");

            // Convert answer to lowercase
            for (size_t i = 0; i < answer.length(); i++)
            {
                answer[i] = static_cast<char>(
                    tolower(static_cast<unsigned char>(answer[i]))
                );
            }

            bool correctAnswer;

            if (answer == "true")
                correctAnswer = true;
            else if (answer == "false")
                correctAnswer = false;
            else
                throw runtime_error("Invalid true/false answer.");

            questions.push_back(
                unique_ptr<Question>(
                    new TrueFalseQuestion(
                        questionId,
                        questionText,
                        marks,
                        correctAnswer
                    )
                )
            );
        }

        // Load a fill-in-the-blank question
        else if (type == "FILL_BLANK")
        {
            string answer;

            if (!getline(in, answer))
                throw runtime_error("Could not read fill-in-the-blank answer.");

            questions.push_back(
                unique_ptr<Question>(
                    new FillBlankQuestion(
                        questionId,
                        questionText,
                        marks,
                        answer
                    )
                )
            );
        }

        // Load a subjective question
        else if (type == "SUBJECTIVE")
        {
            string modelAnswer;

            if (!getline(in, modelAnswer))
                throw runtime_error("Could not read subjective model answer.");

            questions.push_back(
                unique_ptr<Question>(
                    new SubjectiveQuestion(
                        questionId,
                        questionText,
                        marks,
                        modelAnswer
                    )
                )
            );
        }

        else
        {
            throw runtime_error("Unknown question type: " + type);
        }

        // Recalculate the total marks
        maxMarks += marks;
    }

    if (in.bad())
        throw runtime_error("An error occurred while reading the exam file.");
}


// Adds a question to the examination
void Exam::addQuestion(unique_ptr<Question> question)
{
    if (!question)
        throw invalid_argument("Cannot add a null question.");

    // Prevent duplicate question IDs
    for (size_t i = 0; i < questions.size(); i++)
    {
        if (questions[i]->getQuestionId() ==
            question->getQuestionId())
        {
            throw invalid_argument("A question with this ID already exists.");
        }
    }

    double questionMarks = question->getMaxMarks();

    questions.push_back(move(question));
    maxMarks += questionMarks;

    saveExamToFile();
}


// Removes a question using its ID
bool Exam::removeQuestion(int questionId)
{
    for (size_t i = 0; i < questions.size(); i++)
    {
        if (questions[i]->getQuestionId() == questionId)
        {
            questions.erase(questions.begin() + i);

            // Recalculate total marks
            maxMarks = 0.0;

            for (size_t j = 0; j < questions.size(); j++)
            {
                maxMarks += questions[j]->getMaxMarks();
            }

            saveExamToFile();
            return true;
        }
    }

    return false;
}


// Returns the exam ID
int Exam::getExamId() const
{
    return examId;
}


// Returns the exam title
string Exam::getExamName() const
{
    return title;
}


// Returns the total maximum marks
double Exam::getTotalMarks() const
{
    return maxMarks;
}


// Returns the subject
string Exam::getSubject() const
{
    return subject;
}

//Return subjectId
int Exam::getSubjectId() const {
    return subjectId;
}

// Returns the exam date
string Exam::getDate() const
{
    return date;
}


// Returns the duration in minutes
int Exam::getDuration() const
{
    return time;
}


// Displays examination details
void Exam::displayExam() const
{
    cout << "\n========== EXAM DETAILS ==========\n";
    cout << "Exam ID       : " << examId << '\n';
    cout << "Title         : " << title << '\n';
    cout << "Subject       : " << subject << '\n';
    cout << "Date          : " << date << '\n';
    cout << "Duration      : " << time << " minutes\n";
    cout << "Total Marks   : " << maxMarks << '\n';
    cout << "No. Questions : " << questions.size() << '\n';
    cout << "==================================\n";
}


// Displays all questions
void Exam::displayQuestions() const
{
    if (questions.empty())
    {
        cout << "No questions have been added to this exam.\n";
        return;
    }

    cout << "\n========== EXAM QUESTIONS ==========\n";

    for (size_t i = 0; i < questions.size(); i++)
    {
        cout << "\nQuestion " << i + 1 << ":\n";
        cout << *questions[i] << '\n';
    }

    cout << "====================================\n";
}


// Provides read-only access to questions for other components
const vector<unique_ptr<Question>>& Exam::getQuestions() const
{
    return questions;
}
