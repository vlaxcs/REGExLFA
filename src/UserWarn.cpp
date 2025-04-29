#include "UserWarn.h"

void UserWarn::configurationError(const std::string& reason, const std::string& line) {
    if (!line.empty()) {
        std::cout << "Issue at: " << line << std::endl;
    }
    std::cerr << "Configuration error: " << reason << std::endl;
    exit(1);
}

UserWarn::UserWarn(const std::string& reason) {
    UserWarn::configurationError(reason);
}

UserWarn::UserWarn(const std::string& reason, const std::string& line) {
    UserWarn::configurationError(reason, line);
}