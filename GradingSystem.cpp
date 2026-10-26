#include "GradingSystem.h"
#include <cmath>
using namespace std;
bool GradingSystem::isValidMarks(double marksObtained, double maximumMarks) const {
    return std::isfinite(marksObtained) && std::isfinite(maximumMarks)
        && maximumMarks > 0.0 && marksObtained >= 0.0
        && marksObtained <= maximumMarks;
}
bool GradingSystem::isValidMarks(int marksObtained, int maximumMarks) const {
    return isValidMarks(static_cast<double>(marksObtained),
                       static_cast<double>(maximumMarks));
}
bool GradingSystem::calculatePercentage(double marksObtained, double maximumMarks,double& percentage) const {
    if (!isValidMarks(marksObtained, maximumMarks)) {
        return false;
    }
    percentage = marksObtained * 100.0 / maximumMarks;
    return true;
}
bool GradingSystem::calculatePercentage(int marksObtained, int maximumMarks,double& percentage) const {
    return calculatePercentage(static_cast<double>(marksObtained),
                              static_cast<double>(maximumMarks), percentage);
}
bool GradingSystem::calculateGrade(double percentage, string& grade) const {
    if (!std::isfinite(percentage) || percentage < 0.0 || percentage > 100.0) {
        return false;
    }
    if (percentage >= 90.0) {
        grade = "A+";
    } else if (percentage >= 80.0) {
        grade = "A";
    } else if (percentage >= 70.0) {
        grade = "B+";
    } else if (percentage >= 60.0) {
        grade = "B";
    } else if (percentage >= 35.0) {
        grade = "C";
    } else {
        grade = "F";
    }
    return true;
}
bool GradingSystem::checkPassFail(double percentage, bool& passed) const {
    if (!std::isfinite(percentage) || percentage < 0.0 || percentage > 100.0) {
        return false;
    }
    passed = percentage >= 35.0;
    return true;
}