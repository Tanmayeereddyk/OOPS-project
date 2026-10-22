
#ifndef TRUEFALSEQUESTION_H
#define TRUEFALSEQUESTION_H

#include "Question.h"
#include <string>

class TrueFalseQuestion : public Question
{
private:
    bool correctAnswer;

public:
    // Constructor
    TrueFalseQuestion(int id,
                      const std::string& text,
                      double marks,
                      bool answer);

    // Override virtual functions
    void display(std::ostream& out) const override;
    void input(std::istream& in) override;
    void saveToFile(ostream& out) const override;
    // Getter
    bool getCorrectAnswer() const;
};

#endif
