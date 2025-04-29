#pragma once

#include <set>
#include <sstream>

#include "StateNode.h"

class StateNode;
class StateAssembler;

class StateCluster {
private:
    std::set<std::shared_ptr<StateNode>> states;
    std::set<int> state_ids;

public:

    StateCluster() = default;
    explicit StateCluster(const std::set<int>& ids) : state_ids(ids) {}

    std::set<std::shared_ptr<StateNode>> getStates() {
        return states;
    }

    explicit StateCluster(const std::vector<std::shared_ptr<StateNode>>& _states) {
        for (const auto& state : _states) {
            states.insert(state);
            state_ids.insert(state->id);
        }
    }

    bool containsState(const std::shared_ptr<StateNode>& node) const {
        return state_ids.contains(node->id);
    }

    [[nodiscard]] StateCluster unionize(const StateCluster& other) const {
        std::vector<std::shared_ptr<StateNode>> combined(states.begin(), states.end());
        combined.insert(combined.end(), other.states.begin(), other.states.end());
        return StateCluster(combined);
    }

    bool operator!=(const StateCluster& other) const {
        return !(*this == other);
    }

    bool operator==(const StateCluster& other) const {
        return state_ids == other.state_ids;
    }

    bool operator<(const StateCluster& other) const {
        return state_ids < other.state_ids;
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

    [[nodiscard]] bool isEmpty() const {
        return states.empty();
    }

    [[nodiscard]] StateCluster stepWith(const char symbol, const std::vector<std::shared_ptr<StateNode>>& id_map) const {
        std::vector<std::shared_ptr<StateNode>> result;

        for (int id : state_ids) {
            auto& node = id_map[id];
            for (const auto& [target, transition_symbol] : node->connections) {
                if (transition_symbol == symbol) {
                    result.push_back(target);
                }
            }
        }

        return StateCluster(result);
    }
};