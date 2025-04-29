#include <cassert>
#include <sstream>

#include <FiniteAutomaton.h>
#include <State.h>
#include <UserWarn.h>
#include <Setup.h>
#include <RegToken.h>
#include <StateAssembler.h>
#include <queue>
#include <unordered_map>
#include <map>

FiniteAutomaton::FiniteAutomaton(const std::string &file) {
    const Setup setup(file);
    setSigma(setup.getSigma());
    setStates(setup.getStates());
    setTransitions(setup.getTransitions());
    setStartState();
}

std::ostream &operator<<(std::ostream &os, const FiniteAutomaton &fa) {
    if (fa.isNondeterministic()) {
        std::cout << "> NFA" << std::endl;
    }
    else {
        std::cout << "> DFA" << std::endl;
    }

    os << "> Sigma: {";
    bool first = true;
    for (const char &c: fa.sigma) {
        if (!first) os << ", ";
        os << "'" << c << "'";
        first = false;
    }
    os << "}\n";

    first = true;
    os << "> States: {";
    for (const auto &state: fa.states) {
        if (!first) os << ", ";
        os << "'" << state->name << "'";
        first = false;
    }
    os << "}\n";

    os << "> State Map:\n";
    for (const auto &[name, state]: fa.stateMap) {
        os << "  \"" << name << "\": " << state->name << "\n";
    }

    os << "\nStart State: ";
    if (fa.startState) {
        os << fa.startState->name;
    } else {
        os << "nullptr";
    }

    return os;
}

bool FiniteAutomaton::inSigma(const char &symbol) const {
    return this->sigma.contains(symbol);
}

void FiniteAutomaton::setSigma(std::vector<std::string> const &sigma_) {
    for (const auto &line: sigma_) {
        this->sigma.insert(line[0]);
    }
}

void FiniteAutomaton::setStates(const std::vector<std::string> &stateLines) {
    bool hasInitialState = false, hasFinalState = false;
    for (const auto &line: stateLines) {
        auto newState = std::make_shared<State>();

        std::istringstream iss(line);
        std::string token;

        while (std::getline(iss, token, ',')) {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);

            if (token == "S") {
                if (hasInitialState) {
                    UserWarn("Initial state should be unique", line);
                }
                hasInitialState = true;
                newState->initial = true;
            } else if (token == "F") {
                newState->final = true;
                hasFinalState = true;
            } else {
                newState->name = token;
            }
        }

        if (newState->name.empty()) {
            UserWarn("State must have a name", line);
            continue;
        }

        this->stateMap[newState->name] = newState;
        this->states.push_back(newState);
    }

    if (this->states.empty()) {
        UserWarn("There are no states declared for this DFA");
    }

    if (!hasFinalState) {
        UserWarn("At least one final state required");
    }
}

void FiniteAutomaton::setSigma(const std::unordered_set<char> &sigma) {
    this->sigma = sigma;
}

void FiniteAutomaton::setTransitions(std::vector<std::string> const &transitions) {
    for (const auto &line: transitions) {
        std::istringstream iss(line);
        std::string fromState, toState, token;
        char symbol;

        int tokenCount = 0;
        while (std::getline(iss, token, ',')) {
            token.erase(0, token.find_first_not_of(' '));
            token.erase(token.find_last_not_of(' ') + 1);
            switch (tokenCount++) {
                case 0: fromState = token;
                    break;
                case 1: if (token.length() > 1) UserWarn("The symbol should be an unique character", line);
                    symbol = token[0];
                    break;
                case 2: toState = token;
                    break;
                default: break;
            }
        }

        if (!inSigma(symbol)) {
            UserWarn("The symbol is not defined in Sigma");
        }

        if (this->stateMap.contains(fromState) && this->stateMap.contains(toState)) {
            this->stateMap[fromState]->transitions.insert({symbol, this->stateMap[toState]});
        } else {
            UserWarn("There are undefined states", line);
        }
    }
}

void FiniteAutomaton::setStartState() {
    for (const auto &state: states) {
        if (state->initial) {
            startState = state;
            break;
        }
    }
}

bool FiniteAutomaton::isNondeterministic() const {
    for (const auto &state: this->states) {
        for (const auto &symbol: this->sigma) {
            auto [from, with] = state->transitions.equal_range(symbol);
            if (const size_t count = std::distance(from, with); count > 1) return true;
        }
    }
    return false;
}

void FiniteAutomaton::DFS(
    const std::shared_ptr<StateNode> &origin,
    const std::function<void(std::shared_ptr<StateNode>)> &action,
    const std::function<bool(const std::pair<std::shared_ptr<StateNode>, std::optional<char> > &)> &validate) {
    std::unordered_set<int> visited;
    std::stack<std::shared_ptr<StateNode> > toExplore;

    if (!origin) return;
    toExplore.push(origin);

    while (!toExplore.empty()) {
        const auto node = toExplore.top();
        toExplore.pop();

        if (!node || visited.contains(node->id)) continue;

        visited.insert(node->id);
        action(node);

        for (const auto &[target, symbol]: node->connections) {
            if (!target) continue;

            std::optional<char> optSymbol = (symbol == '\0') ? std::nullopt : std::optional<char>(symbol);
            if (!visited.contains(target->id) && validate({target, optSymbol})) {
                toExplore.push(target);
            }
        }
    }
}

std::vector<StateCluster> FiniteAutomaton::LambdaScope(const std::vector<std::shared_ptr<StateNode> > &nodes) {
    std::vector<StateCluster> cluster_states;
    cluster_states.reserve(nodes.size());

    for (const auto &node: nodes) {
        StateAssembler builder;

        DFS(node,
            [&](const std::shared_ptr<StateNode> &current) {
                builder.appendState(current);
            },
            [](const std::pair<std::shared_ptr<StateNode>, std::optional<char> > &connection) {
                return !connection.second.has_value();
            });

        cluster_states.push_back(builder.build());
    }

    return cluster_states;
}

std::unordered_set<char> FiniteAutomaton::extractSigmaFromRegex(const std::string &postfix) {
    std::unordered_set<char> result;
    const std::unordered_set<char> symbols = {'+', '|', '(', ')', '?', '.', '*'};
    for (auto c: postfix) {
        if (symbols.contains(c)) {
            continue;
        }
        result.insert(c);
    }
    return result;
}


static void showTokenization(const std::shared_ptr<RegToken> &token) {
    std::unordered_set<int> visited;
    std::stack<std::pair<std::shared_ptr<StateNode>, int> > toExplore;

    toExplore.emplace(token->startNode, 0);

    while (!toExplore.empty()) {
        auto [node, indent] = toExplore.top();
        toExplore.pop();

        if (!node || visited.contains(node->id)) continue;
        visited.insert(node->id);

        for (const auto &[otherNode, symbol]: node->connections) {
            std::string indentStr(indent, ' ');
            std::cout << std::format("{} {} - {} > {}",
                                     indentStr,
                                     node->toString(),
                                     symbol == '\0' ? 'L' : symbol,
                                     otherNode->toString()
            ) << std::endl;

            if (!visited.contains(otherNode->id)) {
                toExplore.emplace(otherNode, indent + 1);
            }
        }
    }
}

FiniteAutomaton *FiniteAutomaton::buildFromRegex(const std::string &postfix) {
    auto temp = new FiniteAutomaton();
    temp->setSigma(extractSigmaFromRegex(postfix));

    int node_count = 0;
    auto postfix_token = RegToken::getENFAToken(postfix, node_count);
    std::vector<std::shared_ptr<StateNode> > nodes(node_count);

    showTokenization(postfix_token);

    DFS(postfix_token->startNode,
        [&](const std::shared_ptr<StateNode> &current) {
            nodes[current->id] = current;
        },
        [](const std::pair<std::shared_ptr<StateNode>, std::optional<char> > &) {
            return true;
        });

    std::vector<StateCluster> cluster_states = LambdaScope(nodes);
    std::vector<std::vector<StateCluster> > expansion_table(
        node_count, std::vector<StateCluster>(temp->getSigma().size()));
    std::vector<char> sigma_vec(temp->getSigma().begin(), temp->getSigma().end());

    for (int i = 0; i < node_count; ++i) {
        const auto &current = cluster_states[i];

        for (size_t j = 0; j < sigma_vec.size(); ++j) {
            char symbol = sigma_vec[j];
            StateCluster step = current.stepWith(symbol, nodes);
            StateCluster result;

            for (const auto &state: step.getStates()) {
                result = result.unionize(cluster_states[state->id]);
            }

            expansion_table[i][j] = result;
        }
    }

    std::map<StateCluster, int> transition_indices;
    transition_indices[cluster_states[postfix_token->startNode->id]] = 0;

    std::vector<std::vector<int> > transition_rules;
    std::queue<StateCluster> q;
    q.push(cluster_states[postfix_token->startNode->id]);

    while (!q.empty()) {
        StateCluster current = q.front();
        q.pop();

        std::vector<int> row(sigma_vec.size(), -1);
        int current_index = static_cast<int>(transition_rules.size());
        transition_rules.push_back(row);

        for (size_t i = 0; i < sigma_vec.size(); ++i) {
            StateCluster result;
            for (const auto &state: current.getStates()) {
                result = result.unionize(expansion_table[state->id][i]);
            }

            if (result.isEmpty()) continue;

            if (!transition_indices.contains(result)) {
                int new_index = static_cast<int>(transition_rules.size() + q.size());
                transition_indices[result] = new_index;
                q.push(result);
            }

            transition_rules[current_index][i] = transition_indices[result];
        }
    }

    std::vector<std::shared_ptr<State>> fa_states(transition_rules.size());
    for (size_t i = 0; i < fa_states.size(); ++i) {
        auto state = std::make_shared<State>();
        state->name = "q" + std::to_string(i);
        state->initial = (i == 0);
        // temp->stateMap[state->name] = state;
        fa_states[i] = state;
    }

    for (size_t from = 0; from < transition_rules.size(); ++from) {
        for (size_t j = 0; j < sigma_vec.size(); ++j) {
            if (int to = transition_rules[from][j]; to != -1) {
                fa_states[from]->transitions.emplace(sigma_vec[j], fa_states[to]);
            }
        }
    }

    for (const auto &[cluster, index]: transition_indices) {
        if (cluster.containsState(postfix_token->endNode)) {
            fa_states[index]->final = true;
        }
    }

    temp->states = fa_states;
    temp->startState = fa_states[0];

    return temp;
}

bool FiniteAutomaton::process(const std::string& word) const {
    auto currentState = startState;
    assert(currentState != nullptr);

    if (word.empty()) {
        return currentState->final;
    }

    for (const auto &symbol : word) {
        auto transitionsWithSymbol = currentState->transitions.equal_range(symbol);
        if (transitionsWithSymbol.first == transitionsWithSymbol.second) {
            return false;
        }
        currentState = transitionsWithSymbol.first->second;
    }

    return currentState->final;
}

// To implement (Optional)
// FiniteAutomaton* FiniteAutomaton::getMinimizedVersion() {
//     if (!isNondeterministic()) {
//         return nullptr;
//     }
//
//     return nullptr;
// }
