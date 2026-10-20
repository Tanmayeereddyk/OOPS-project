#ifndef VALIDATION_H
#define VALIDATION_H
#include <string>
namespace Validation {
    // The character that separates fields in the data files
    const char DELIMITER = '|';
    // Removes spaces at the start and end of the text
    std::string trimText(const std::string& text);
    // True if the text contains '|'
    bool hasDelimiter(const std::string& text);
    // Compares two texts, ignoring upper/lower case
    bool sameIgnoreCase(const std::string& a, const std::string& b);
    // One '@', a '.' after it, no '|'
    bool isValidEmail(const std::string& email);
    // Asks until the user enters a whole number between min and max
    int readNumber(const std::string& prompt, int min, int max);
    // Asks until the user enters non-empty text without '|' (returns it trimmed)
    std::string readText(const std::string& prompt);
}
#endif