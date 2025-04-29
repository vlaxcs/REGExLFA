#pragma once

#include <vector>
#include <string>
struct State;

using Edge = State;

struct State {
    char symbol = 0;
    Edge* firstEdge = nullptr;
    Edge* secondEdge = nullptr;
};

// Struct representing a non-deterministic finite automaton (NFA)
struct NFA {
    State* initial = nullptr;
    State* accept = nullptr;

    NFA(State* i, State* a) : initial(i), accept(a) {}
};

// Converts a regular expression in postfix notation into an NFA
inline NFA* regexToNfa(const std::string& postfix) {
    std::vector<NFA*> nfaStack;

    for (const char r : postfix) {
        switch (r) {

            // Concatenation
            case '.': {
                const NFA* frag2 = nfaStack.back(); nfaStack.pop_back();
                const NFA* frag1 = nfaStack.back(); nfaStack.pop_back();

                frag1->accept->firstEdge = frag2->initial;
                auto newNfa = new NFA(frag1->initial, frag2->accept);
                nfaStack.push_back(newNfa);
                break;
            }

            // Alternation (OR)
            case '|': {
                const NFA* frag2 = nfaStack.back(); nfaStack.pop_back();
                const NFA* frag1 = nfaStack.back(); nfaStack.pop_back();

                auto* initial = new State();
                auto* accept = new State();

                initial->firstEdge = frag1->initial;
                initial->secondEdge = frag2->initial;

                frag1->accept->firstEdge = accept;
                frag2->accept->firstEdge = accept;

                nfaStack.push_back(new NFA(initial, accept));
                break;
            }

            // Zero or one (?)
            case '?': {
                const NFA* frag = nfaStack.back(); nfaStack.pop_back();

                auto* initial = new State();
                auto* accept = new State();

                initial->firstEdge = frag->initial;
                initial->secondEdge = accept;

                frag->accept->firstEdge = accept;

                nfaStack.push_back(new NFA(initial, accept));
                break;
            }

            // One or more (+)
            case '+': {
                const NFA* frag = nfaStack.back(); nfaStack.pop_back();

                auto* accept = new State();
                auto* initial = new State();

                initial->firstEdge = frag->initial;
                initial->secondEdge = accept;

                frag->accept->firstEdge = initial;

                nfaStack.push_back(new NFA(frag->initial, accept));
                break;
            }

            // Zero or more (*)
            case '*': {
                const NFA* frag = nfaStack.back(); nfaStack.pop_back();

                auto* accept = new State();
                auto* initial = new State();

                initial->firstEdge = frag->initial;
                initial->secondEdge = accept;

                frag->accept->firstEdge = frag->initial;
                frag->accept->secondEdge = accept;

                nfaStack.push_back(new NFA(initial, accept));
                break;
            }

            // Literal character
            default: {
                auto* accept = new State();
                auto* initial = new State();
                initial->symbol = r;
                initial->firstEdge = accept;

                nfaStack.push_back(new NFA(initial, accept));
                break;
            }
        }
    }

    return nfaStack.empty() ? nullptr : nfaStack[0];
}

// Helper function to add a state (and all reachable ε-transitions)
inline std::vector<State*> addState(std::vector<State*> states, State* state, State* accept) {
    if (state == nullptr) return states;

    states.push_back(state);

    if (state != accept && state->symbol == 0) { // Epsilon transition
        states = addState(states, state->firstEdge, accept);
        if (state->secondEdge != nullptr) {
            states = addState(states, state->secondEdge, accept);
        }
    }
    return states;
}

bool matches(const std::string& regex, const std::string& input);

// --- You need to define infixToPostfix and regexToNfa beforehand ---

// Stack helper to pop an element
std::pair<std::vector<char>, char> pop(std::vector<char> stack) {
    char r = stack.back();
    stack.pop_back();
    return {stack, r};
}

// Main matching function
bool matches(const std::string& regex, const std::string& input) {
    bool isMatch = false;

    std::string postfix = infixToPostfix(regex); // <- you need to define this
    NFA* regexNfa = regexToNfa(postfix);          // <- already defined

    std::vector<State*> current;
    std::vector<State*> next;

    current = addState({}, regexNfa->initial, regexNfa->accept);

    for (char r : input) {
        for (State* c : current) {
            if (c->symbol == r) {
                next = addState(next, c->firstEdge, regexNfa->accept);
            }
        }
        current = next;
        next.clear();
    }

    for (State* c : current) {
        if (c == regexNfa->accept) {
            isMatch = true;
            break;
        }
    }

    return isMatch;
}