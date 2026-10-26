#ifndef FILLBLANKQUESTION_H
#define FILLBLANKQUESTION_H
#include "Question.h"
#include <string>
class FillBlankQuestion : public Question
{
private:
    std::string correctAnswer;
public:
    FillBlankQuestion(int id,
                      const std::string& text,
                      double marks,
                      const std::string& answer);

    void display(std::ostream& out) const override;
    void input(std::istream& in) override;
    void saveToFile(ostream& out) const override;
    std::string getCorrectAnswer() const;
};
#endif