#pragma once

#include <string>
#include <iostream>

class UserWarn {
    static void configurationError(const std::string& reason, const std::string& line = "");

public:
    explicit UserWarn(const std::string& reason);
    explicit UserWarn(const std::string& reason, const std::string& line);
    ~UserWarn() = default;
};