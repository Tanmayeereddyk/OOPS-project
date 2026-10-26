#ifndef CLASSREPORT_H
#define CLASSREPORT_H
#include <iostream>
#include <string>
#include "AttemptStorage.h"
class ClassReport {
public:
    ClassReport();
    bool generateReport(int examId,
                        const std::string& className,
                        const std::string& section,
                        std::ostream& out = std::cout,
                        const std::string& attemptFilePath = AttemptStorage::defaultFilePath()) const;
};
#endif