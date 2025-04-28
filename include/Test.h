#pragma once

#include <unordered_map>

struct Test {
    std::string regex;
    std::vector<std::pair<std::string, bool>> test_strings;
};