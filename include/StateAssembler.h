#pragma once

#include <vector>
#include "StateCluster.h"

class StateAssembler {
private:
    std::vector<std::shared_ptr<StateNode>> states;

public:
    StateAssembler() = default;

    void appendState(std::shared_ptr<StateNode> state);

    void appendState(const StateCluster& nodes);

    [[nodiscard]] StateCluster makeStep(char c = '\0') const {
        StateAssembler assembler;
        for (const auto&state : states) {
            for (const auto&[from, with] : state->connections) {
                if (with == c) {
                    assembler.appendState(from);
                }
            }
        }
        return assembler.build();
    }

    [[nodiscard]] StateCluster build() const;
};