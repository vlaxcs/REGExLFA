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

    void makeTests(const std::string &filename);

    Tester() = default;

public:
    static Tester &getInstance() {
        static Tester instance;
        return instance;
    }

    Tester(const Tester &) = delete;

    Tester &operator=(const Tester &) = delete;

    std::string getTestDirectory();

    void clearTests();

    void init();

    void run();
};
