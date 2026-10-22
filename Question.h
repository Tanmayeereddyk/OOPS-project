#ifndef QUESTION_H
#define QUESTION_H
#include <string>
#include <iostream>
using namespace std;
class Question{
    private:
        int questionId;
        string questionText;
        double maxMarks;
    public:
      //Constructor
       Question(int id, const string& text, double marks);
      //Virtual Destructor
       virtual ~Question();
 
       virtual void display(ostream& out) const=0;
       virtual void input(istream& in)=0;
       virtual void saveToFile(ostream& out) const = 0;


       //operator overloading
       friend ostream& operator<<(ostream& out, const Question& q);
       friend istream& operator>>(istream& in, Question& q);

       //Getters
      int getQuestionId() const;
      string getQuestionText() const;
      double getMaxMarks() const;
    protected:
      //Setters
        void setQuestionId(int id);
        void setQuestionText(const string& text);
        void setMaxMarks(double marks);
};
#endif