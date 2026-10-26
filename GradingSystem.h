#ifndef GRADINGSYSTEM_H
#define GRADINGSYSTEM_H
#include <string>
// Applies the project's fixed grading rules to whole-number marks.
class GradingSystem {
public:
    // Returns true only when maximumMarks is positive and the obtained marks
    // are between zero and maximumMarks, inclusive.
    bool isValidMarks(double marksObtained, double maximumMarks) const;
    bool isValidMarks(int marksObtained, int maximumMarks) const;
    // On success, writes the unrounded percentage to percentage.
    // Returns false for invalid marks and leaves percentage unchanged.
    bool calculatePercentage(double marksObtained, double maximumMarks,
                             double& percentage) const;
    bool calculatePercentage(int marksObtained, int maximumMarks,
                             double& percentage) const;
    // Converts a percentage from 0 through 100 to its grade.
    // Returns false for an invalid percentage and leaves grade unchanged.
    bool calculateGrade(double percentage, std::string& grade) const;
    // Sets passed according to the 35% threshold.
    // Returns false for an invalid percentage and leaves passed unchanged.
    bool checkPassFail(double percentage, bool& passed) const;
};
#endif