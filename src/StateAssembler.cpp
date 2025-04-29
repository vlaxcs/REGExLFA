#include "StateAssembler.h"
#include "StateCluster.h"

void StateAssembler::appendState(std::shared_ptr<StateNode> state) {
    states.push_back(std::move(state));
}

void StateAssembler::appendState(const StateCluster& nodes) {
    for (const auto& node : nodes) {
        states.push_back(node);
    }
}

StateCluster StateAssembler::build() const {
    return StateCluster(states);
}