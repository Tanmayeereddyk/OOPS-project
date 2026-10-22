
#ifndef MCQQUESTION_H
#define MCQQUESTION_H

#include "Question.h"
#include <vector>
#include <string>
using namespace std;
class MCQQuestion : public Question
{
private:
    vector<string> options;
    int correctOption;//1 based index of the correct option (1, 2, 3, or 4)

public:
    // Constructor
    MCQQuestion(int id,
                const string& text,
                double marks,
                const vector<string>& opts,
                int correct);

    // Override virtual functions
    void display(ostream& out) const override;
    void input(istream& in) override;
    void saveToFile(ostream& out) const override;



    // Getter functions
    vector<string> getOptions() const;
    int getCorrectOption() const;
};

#endif
