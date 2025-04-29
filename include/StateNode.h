#pragma once

#include <vector>
#include <format>
#include <memory>

class StateNode;
class StateCluster;

class StateNode {
public:
    int id;
    std::vector<std::pair<std::shared_ptr<StateNode>, char>> connections;

    explicit StateNode(const int id) : id(id) {};

    void appendConnection(std::shared_ptr<StateNode> node, char connection = '\0') {
        connections.emplace_back(std::move(node), connection);
    }

    bool operator<(const StateNode& node) const {
        return this->id < node.id;
    }

    [[nodiscard]] std::string toString() const {
        std::string result = std::format("({})", id);
        return result;
    }
};