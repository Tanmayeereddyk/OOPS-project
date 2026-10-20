#include "Validation.h"
#include <cctype>
#include <cstdlib>
#include <iostream>
namespace Validation {
std::string trimText(const std::string& text) {
    const std::string spaces = " \t\r\n";
    size_t start = text.find_first_not_of(spaces);
    if (start == std::string::npos) {
        return "";   // text is empty or only spaces
    }
    size_t end = text.find_last_not_of(spaces);
    return text.substr(start, end - start + 1);
}
bool hasDelimiter(const std::string& text) {
    return text.find(DELIMITER) != std::string::npos;
}
bool sameIgnoreCase(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t i = 0; i < a.size(); i++) {
        if (std::tolower(static_cast<unsigned char>(a[i])) !=
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    return true;
}
bool isValidEmail(const std::string& email) {
    if (email.empty() || hasDelimiter(email)) {
        return false;
    }
    if (email.find(' ') != std::string::npos) {
        return false;   // no spaces allowed
    }
    // Exactly one '@', and not at the start
    size_t at = email.find('@');
    if (at == std::string::npos || at == 0) {
        return false;
    }
    if (email.find('@', at + 1) != std::string::npos) {
        return false;   // second '@'
    }
    // A '.' after the '@', with something between them and something after it
    size_t dot = email.find('.', at + 1);
    if (dot == std::string::npos || dot == at + 1 || dot == email.size() - 1) {
        return false;
    }
    return true;
}
int readNumber(const std::string& prompt, int min, int max) {
    while (true) {
        std::cout << prompt;
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "\nInput closed. Exiting.\n";
            std::exit(1);   // avoids an endless loop if input ends
        }
        input = trimText(input);
        try {
            size_t used = 0;
            int number = std::stoi(input, &used);
            if (used == input.size() && number >= min && number <= max) {
                return number;
            }
        } catch (...) {
            // not a number, fall through to the message below
        }
        std::cout << "Please enter a number between " << min << " and " << max << ".\n";
    }
}
std::string readText(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string input;
        if (!std::getline(std::cin, input)) {
            std::cout << "\nInput closed. Exiting.\n";
            std::exit(1);
        }
        input = trimText(input);
        if (input.empty()) {
            std::cout << "This cannot be empty.\n";
        } else if (hasDelimiter(input)) {
            std::cout << "The '|' character is not allowed.\n";
        } else {
            return input;
        }
    }
}
}