#include <TestFramework.h>

#include <iostream>

namespace Testing
{
    namespace
    {
        // Tracks whether the currently running test has failed.
        bool currentTestFailed = false;
    } // namespace

    std::vector<TestCase>& GetTestRegistry()
    {
        static std::vector<TestCase> registry;
        return registry;
    }

    int RegisterTest(const std::string& suite, const std::string& name, TestFunction function)
    {
        GetTestRegistry().push_back(TestCase{suite, name, function});
        return 0;
    }

    void ReportFailure(const char* file, int line, const std::string& message)
    {
        std::cout << "    " << file << ":" << line << ": failure: " << message << "\n";
        currentTestFailed = true;
    }

    int RunAllTests()
    {
        int passedCount = 0;
        int failedCount = 0;

        for (const TestCase& testCase : GetTestRegistry())
        {
            currentTestFailed = false;
            std::cout << "[ RUN  ] " << testCase.suite << "." << testCase.name << "\n";

            testCase.function();

            if (currentTestFailed)
            {
                std::cout << "[ FAIL ] " << testCase.suite << "." << testCase.name << "\n";
                ++failedCount;
            }
            else
            {
                std::cout << "[ PASS ] " << testCase.suite << "." << testCase.name << "\n";
                ++passedCount;
            }
        }

        std::cout << "\n"
                  << passedCount << " passed, " << failedCount << " failed\n";
        return failedCount;
    }
} // namespace Testing
