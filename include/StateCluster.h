#pragma once

#include <set>
#include <sstream>

#include "StateNode.h"

class StateNode;
class StateAssembler;

class StateCluster {
private:
    std::set<std::shared_ptr<StateNode>> states;

public:
    StateCluster() = default;

    explicit StateCluster(const std::vector<std::shared_ptr<StateNode>>& _states) {
        for (const auto& state : _states) {
            states.insert(state);
        }
    }

    [[nodiscard]] bool containsState(std::shared_ptr<StateNode> const& state) const {
        return states.contains(state);
    }

    [[nodiscard]] StateCluster unionize(const StateCluster& other) const {
        std::vector<std::shared_ptr<StateNode>> combined(states.begin(), states.end());
        combined.insert(combined.end(), other.states.begin(), other.states.end());
        return StateCluster(combined);
    }

    bool operator==(const StateCluster& other) const {
        return states == other.states;
    }

    bool operator!=(const StateCluster& other) const {
        return !(*this == other);
    }

    [[nodiscard]] std::string toString() const {
        std::ostringstream oss;
        oss << "{";
        bool first = true;
        for (const auto& state : states) {
            if (!first) oss << ",";
            oss << state->toString();
            first = false;
        }
        oss << "}";
        return oss.str();
    }

    [[nodiscard]] auto begin() const { return states.begin(); }
    [[nodiscard]] auto end() const { return states.end(); }
};