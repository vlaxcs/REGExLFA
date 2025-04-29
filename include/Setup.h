#pragma once

#include <vector>
#include <string>

class Setup {
    std::vector<std::string> sigma;
    std::vector<std::string> states;
    std::vector<std::string> transitions;

public:
    explicit Setup(const std::string& file);

    [[nodiscard]] const std::vector<std::string>& getSigma() const;
    [[nodiscard]] const std::vector<std::string>& getStates() const;
    [[nodiscard]] const std::vector<std::string>& getTransitions() const;

    ~Setup() = default;
};