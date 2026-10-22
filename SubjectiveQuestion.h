
#ifndef SUBJECTIVEQUESTION_H
#define SUBJECTIVEQUESTION_H

#include "Question.h"
#include <string>

class SubjectiveQuestion : public Question
{
private:
    std::string modelAnswer;

public:
    SubjectiveQuestion(int id,
                       const std::string& text,
                       double marks,
                       const std::string& answer);

    void display(std::ostream& out) const override;
    void input(std::istream& in) override;
    void saveToFile(std::ostream& out) const override;

    std::string getModelAnswer() const;
};

#endif
