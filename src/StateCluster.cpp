#include "StateCluster.h"

StateCluster StateCluster::stepWith(const char symbol, const std::vector<std::shared_ptr<StateNode>>& id_map) const {
    std::vector<std::shared_ptr<StateNode>> result;

    for (const int id : state_ids) {
        for (auto& node = id_map[id]; const auto& [target, transition_symbol] : node->connections) {
            if (transition_symbol == symbol) {
                result.push_back(target);
            }
        }
    }

    return StateCluster(result);
}

std::string StateCluster::toString() const {
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