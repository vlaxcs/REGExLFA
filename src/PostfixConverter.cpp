#include "PostfixConverter.h"

#include <stack>

std::string PostfixConverter::add_concat_symbols(const std::string& expression){
    std::string result;

    for (int i = 0; i < expression.size() - 1; i++) {
        const char t1(expression[i]), t2(expression[i + 1]);
        result += expression[i];

        if((isSymbol(expression[i]) && isSymbol(expression[i + 1]))
             || (isSymbol(expression[i]) && expression[i + 1] == '(')
             || (expression[i] == ')' && isSymbol(expression[i + 1]))
             || (expression[i] == ')' && expression[i + 1] == '(')
             || (isUnary(expression[i]) && (isSymbol(expression[i + 1]) || expression[i + 1] == '('))) {
                result += '.';
        }
    }

    result += expression[expression.size() - 1];
    return result;
}

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
            while (!st.empty() && priority[st.top()] >= priority[c]) {
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
    const std::string result(add_concat_symbols(expression));
    return to_postfix(result);
}