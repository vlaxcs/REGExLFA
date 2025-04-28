#include "Tester.h"

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

    for (const auto& [name, test] : this->tests) {
        std::cout << "Test name: " << name << "\n";
        std::cout << "Regex: " << test.regex << "\n";
        std::cout << "Postfix: " << PostfixConverter::getPostfix(test.regex) << '\n';
        for (const auto& [input, expected] : test.test_strings) {
            std::cout << "  Input: " << input << ", Expected: " << (expected ? "true" : "false") << "\n";
        }
        std::cout << std::endl;
    }
}

void Tester::init() {
    for (const auto& entry : fs::directory_iterator(testDirectory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().string();
            makeTests(filename);
        }
    }
}

void Tester::run() {
    const std::string testDirectory = getTestDirectory();
    clearTests();
}