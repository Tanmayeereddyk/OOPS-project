
#ifndef EXAM_H
#define EXAM_H

#include <string>
#include <vector>
#include <memory>

#include "Question.h"
#include "subject.h"
class Exam
{
private:
    int examId;
    int subjectId;
    std::string title;
    std::string subject;
    std::string date;
    int time;                    // Duration in minutes
    double maxMarks;             // Calculated from questions

    std::vector<std::unique_ptr<Question>> questions;

    // Generates the filename for this exam
    std::string getFileName() const;

    // Creates the file for a new exam
    void createExamFile() const;

    // Saves exam details and all questions
    void saveExamToFile() const;

    // Loads exam details and all questions
    void loadExamFromFile();

public:
    // Creates a new examination
    Exam(int id,
         const std::string& examTitle,
         const std::string& examSubject,
         const std::string& examDate,
         int duration);

    // Opens an existing examination using its ID
    explicit Exam(int id);

    // Prevent copying because the exam owns its questions
    Exam(const Exam&) = delete;
    Exam& operator=(const Exam&) = delete;

    // Adds a question
    void addQuestion(std::unique_ptr<Question> question);

    // Removes a question by ID
    bool removeQuestion(int questionId);

    // Returns exam ID
    int getExamId() const;

    //Returns subject ID
     int getSubjectId() const;

    // Returns exam title
    std::string getExamName() const;

    // Returns total maximum marks
    double getTotalMarks() const;

    // Returns subject
    std::string getSubject() const;

    // Returns exam date
    std::string getDate() const;

    // Returns duration in minutes
    int getDuration() const;

    // Displays exam details
    void displayExam() const;

    // Displays all questions
    void displayQuestions() const;

    // Read-only access for attempt and evaluation components
    const std::vector<std::unique_ptr<Question>>&
    getQuestions() const;
};

#endif
