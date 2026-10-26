#include <iostream>
#include "Exam.h"
#include "GradingSystem.h"
#include "student.h"
#include "studentmanager.h"

int main() {
    std::cout << "====================================\n";
    std::cout << "     EXAMINATION SYSTEM TEST PROGRAM\n";
    std::cout << "====================================\n\n";

    std::cout << "TEST 1: Creating a new exam with a unique ID...\n";
    try {
        // Using ID 102 to avoid collision with pre-existing exam_101.txt file
        Exam exam(102, "Advanced Object-Oriented Programming Test");
        std::cout << "SUCCESS: Exam created successfully with ID: 102\n";
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }

    std::cout << "\nTEST 2: Initializing Student Manager...\n";
    try {
        StudentManager manager;
        std::cout << "SUCCESS: Student Manager initialized and ready.\n";
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << "\n";
    }

    std::cout << "\n====================================\n";
    std::cout << "       TEST PROGRAM COMPLETED        \n";
    std::cout << "====================================\n";

    return 0;
}