#pragma once

#include <fstream>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
using json = nlohmann::json;
namespace fs = std::filesystem;

#include "PostfixConverter.h"
#include "Test.h"

class Tester {
    std::string testDirectory = "../tests";
    std::unordered_map<std::string, Test> tests;

    void makeTests(const std::string& filename);

public:
    Tester() = default;

    std::string getTestDirectory() {
        return testDirectory;
    }
    void clearTests() {
        tests.clear();
    }

    void init();
    void run();
};