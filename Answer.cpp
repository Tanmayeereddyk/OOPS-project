#include "Answer.h"
// Constructor definition
Answer::Answer(int id,
               const std::string& studentResponse,
               double marks,
               bool isEvaluated)
    : questionId(id),
      response(studentResponse),
      marksAwarded(marks),
      evaluated(isEvaluated)
{
}