#include <fstream>
#include <iostream>
#include <stdexcept>

#include "Setup.h"
#include "UserWarn.h"


Setup::Setup(const std::string& file) {
    std::ifstream f(file);
    if (!f.is_open()) {
        throw std::runtime_error("Provided file does not exist: " + file);
    }

    std::string line;
    bool readSigma = false, readStates = false, readTransitions = false;

    while (std::getline(f, line)) {
        if (line.find("Sigma") != std::string::npos) {
            readSigma = true;
            readStates = readTransitions = false;
        } else if (line.find("States") != std::string::npos) {
            readStates = true;
            readSigma = readTransitions = false;
        } else if (line.find("Transitions") != std::string::npos) {
            readTransitions = true;
            readSigma = readStates = false;
        } else if (line.find("End") != std::string::npos) {
            readSigma = readStates = readTransitions = false;
        } else if (line.starts_with("#") || line.empty()) {
            continue;
        } else {
            if (readSigma) {
                sigma.push_back(line);
            } else if (readStates) {
                states.push_back(line);
            } else if (readTransitions) {
                transitions.push_back(line);
            }
        }
    }

    f.close();
}

[[nodiscard]] const std::vector<std::string>& Setup::getSigma() const {
    return sigma;
}

[[nodiscard]] const std::vector<std::string>& Setup::getStates() const {
    return states;
}

[[nodiscard]] const std::vector<std::string>& Setup::getTransitions() const {
    return transitions;
}