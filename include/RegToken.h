#pragma once

#include <stdexcept>
#include <stack>
#include <unordered_set>
#include <memory>
#include <utility>
#include "StateNode.h"

class RegToken {
private:
    static bool isBinary(const char c) {
        const std::unordered_set<char> unaryOperators = {'|', '.'};
        return unaryOperators.contains(c);
    }

    static bool isOperator(const char c) {
        const std::unordered_set<char> symbols = {'.', '*', '|', '+', '?'};
        return symbols.contains(c);
    }

public:
    std::shared_ptr<StateNode> startNode;
    std::shared_ptr<StateNode> endNode;

    RegToken() = default;

    RegToken(std::shared_ptr<StateNode> startNode, std::shared_ptr<StateNode> endNode)
        : startNode(std::move(startNode)), endNode(std::move(endNode)) {
    }

    RegToken(char symbol, int &i);

    RegToken(int &i, const std::shared_ptr<RegToken> &lhs, char operation = 'x', const std::shared_ptr<RegToken>&rhs = nullptr);

    static std::shared_ptr<RegToken> getENFAToken(const std::string& postfix, int& nodesCount);
};
