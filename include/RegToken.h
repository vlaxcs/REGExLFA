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

    RegToken(char symbol, int &i) {
        startNode = std::make_shared<StateNode>(i++);
        endNode = std::make_shared<StateNode>(i++);
        startNode->appendConnection(endNode, symbol);
    }

    RegToken(int &i, const std::shared_ptr<RegToken> &lhs, char operation = 'x', const std::shared_ptr<RegToken>&rhs = nullptr) {
        switch (operation) {
            case 'x':
                break;
            case '.':
                startNode = lhs->startNode;
                lhs->endNode->appendConnection(rhs->startNode, '\0');
                endNode = rhs->endNode;
                break;

            case '|':
                startNode = std::make_shared<StateNode>(i++);
                endNode = std::make_shared<StateNode>(i++);

                startNode->appendConnection(lhs->startNode, '\0');
                startNode->appendConnection(rhs->startNode, '\0');
                lhs->endNode->appendConnection(endNode, '\0');
                rhs->endNode->appendConnection(endNode, '\0');
                break;

            case '*':
                startNode = std::make_shared<StateNode>(i++);
                endNode = std::make_shared<StateNode>(i++);

                startNode->appendConnection(lhs->startNode, '\0');
                lhs->endNode->appendConnection(lhs->startNode, '\0');
                lhs->endNode->appendConnection(endNode, '\0');
                startNode->appendConnection(endNode, '\0');
                break;

            case '?':
                startNode = std::make_shared<StateNode>(i++);
                endNode = std::make_shared<StateNode>(i++);

                startNode->appendConnection(lhs->startNode, '\0');
                lhs->endNode->appendConnection(endNode, '\0');
                startNode->appendConnection(endNode, '\0');
                break;

            case '+':
                startNode = lhs->startNode;
                endNode = lhs->endNode;
                endNode->appendConnection(startNode, '\0');
                break;

            default:
                throw std::runtime_error("Invalid operation character");
        }
    }

    static std::shared_ptr<RegToken> getENFAToken(const std::string& postfix, int& nodesCount) {
        std::stack<std::shared_ptr<RegToken>> current_tokens;

        for (char c : postfix) {
            if (isOperator(c)) {
                std::shared_ptr<RegToken> rhs = nullptr;
                std::shared_ptr<RegToken> lhs = current_tokens.top();
                current_tokens.pop();

                if (isBinary(c)) {
                    rhs = lhs;
                    lhs = current_tokens.top();
                    current_tokens.pop();

                    auto newToken = std::make_shared<RegToken>(nodesCount, lhs, c, rhs);
                    current_tokens.push(newToken);
                } else {
                    auto newToken = std::make_shared<RegToken>(nodesCount, lhs, c, nullptr);
                    current_tokens.push(newToken);
                }
            } else {
                auto newToken = std::make_shared<RegToken>(c, nodesCount);
                current_tokens.push(newToken);
            }
        }

        if (current_tokens.size() != 1) {
            throw std::runtime_error("Postfix expression is wrong");
        }

        return current_tokens.top();
    }
};
