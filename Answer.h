#ifndef ANSWER_H
#define ANSWER_H
#include <string>
struct Answer {
    int questionId;
    std::string response;
    double marksAwarded;
    bool evaluated;
    Answer(int id = 0,const std::string& studentResponse = "",double marks = 0.0,bool isEvaluated = false);
};
#endif