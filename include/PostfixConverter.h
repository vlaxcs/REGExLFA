#pragma once

#include <unordered_set>
#include <string>
#include <stack>
#include <unordered_map>

class PostfixConverter {
private:
    static bool isUnary(const char c);

    static bool isSymbol(const char c);

    [[nodiscard]] static std::string add_concat_symbols(const std::string& expression);
    [[nodiscard]] static std::string to_postfix(const std::string& expression);

public:
    PostfixConverter() = default;
    [[nodiscard]] static std::string getPostfix(const std::string& expression);
};