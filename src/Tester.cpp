#include <format>

#include "Tester.h"
#include "FiniteAutomaton.h"

void Tester::makeTests(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    json testData;
    file >> testData;
    file.close();

    for (const auto& entry : testData) {
        Test test;
        test.regex = entry["regex"];

        for (const auto& test_case : entry["test_strings"]) {
            test.test_strings.emplace_back(test_case["input"], test_case["expected"]);
        }

        tests[entry["name"]] = test;
    }
}

void Tester::init() {
    const std::string testDirectory = getTestDirectory();
    for (const auto& entry : fs::directory_iterator(testDirectory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().string();
            makeTests(filename);
        }
    }
}

void Tester::run() {
    if (this->tests.empty()) {
        std::cerr << "There are no tests. Use Tester::init() before run." << std::endl;
    }
    else {
        for (const auto& [name, test] : this->tests) {
            std::string postfix = PostfixConverter::getPostfix(test.regex);
            FiniteAutomaton* regexParser(FiniteAutomaton::buildFromRegex(postfix));
            std::cout << *regexParser;

            std::cout << "Test name: " << name << std::endl;
            std::cout << "Regex: " << test.regex << std::endl;
            std::cout << std::format("\033[34mPostfix: {}\033[0m", postfix) << std::endl;
            for (const auto& [input, expected] : test.test_strings) {
                if (const auto result = regexParser->process(postfix); result == expected) {
                    std::cout << std::format("\033[32mInput: {} | Expected: {} | Result: {} \033[0m", input, (expected ? "true" : "false"), (result ? "true" : "false")) << std::endl;
                } else {
                    std::cout << std::format("\033[31mInput: {} | Expected: {} | Result: {} \033[0m", input, (expected ? "true" : "false"), (result ? "true" : "false")) << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }

    clearTests();
}