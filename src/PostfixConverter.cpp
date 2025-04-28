#include "PostfixConverter.h"

#include <stack>

// Shunting Yard
std::string PostfixConverter::to_postfix(const std::string& expression) {
    std::unordered_map<char, int> priority {
        {'|', 1},
            {'.', 2},
        {'?', 3},
        {'+', 3},
        {'*', 3}
    };
    std::string result;
    std::stack<char> st;

    for (char c : expression) {
        if (c == '(') {
            st.push(c);
        } else if (c == ')') {
            while (!st.empty() && st.top() != '(') {
                result += st.top();
                st.pop();
            }
            if (!st.empty()) {
                st.pop();
            }
        } else if (priority.contains(c)) {
            while (!st.empty() && st.top() != '(' && priority[st.top()] >= priority[c]) {
                result += st.top();
                st.pop();
            }
            st.push(c);
        } else {
            result += c;
        }
    }

    while (!st.empty()) {
        result += st.top();
        st.pop();
    }

    return result;
}

std::string PostfixConverter::getPostfix(const std::string& expression) {
    // const std::string result(add_concat_symbols(expression));
    return to_postfix(expression);
}