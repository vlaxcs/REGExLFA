#include <sstream>

#include <FiniteAutomaton.h>
#include <State.h>
#include <UserWarn.h>
#include <Setup.h>
#include <RegToken.h>
#include <StateAssembler.h>


FiniteAutomaton::FiniteAutomaton(const std::string &file) {
    const Setup setup(file);
    setSigma(setup.getSigma());
    setStates(setup.getStates());
    setTransitions(setup.getTransitions());
    setStartState();
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

bool FiniteAutomaton::isNondeterministic() {
    for (const auto &state: states) {
        for (const auto &symbol: sigma) {
            auto [from, with] = state->transitions.equal_range(symbol);
            if (const size_t count = std::distance(from, with); count > 1) return true;
        }
    }
    return false;
}


void FiniteAutomaton::DFS(std::shared_ptr<StateNode> origin,
         const std::function<void(std::shared_ptr<StateNode>)>& action,
         const std::function<bool(const std::pair<std::shared_ptr<StateNode>, std::optional<char>>&)>& validate)
{
    std::unordered_set<int> visited;
    std::stack<std::shared_ptr<StateNode>> toExplore;

    toExplore.push(origin);

    while (!toExplore.empty()) {
        auto node = toExplore.top();
        toExplore.pop();

        if (visited.contains(node->id)) continue;
        visited.insert(node->id);
        action(node);

        for (const auto& [target, symbol] : node->connections) {
            std::optional<char> optSymbol = symbol == '\0' ? std::nullopt : std::optional<char>(symbol);
            if (!visited.contains(target->id) && validate({target, optSymbol})) {
                toExplore.push(target);
            }
        }
    }
}

std::vector<StateCluster> FiniteAutomaton::LambdaScope(const std::vector<std::shared_ptr<StateNode>>& nodes) {
    std::vector<StateCluster> compositeStates;
    compositeStates.reserve(nodes.size());

    for (const auto& node : nodes) {
        StateAssembler builder;

        DFS(node,
              [&](const std::shared_ptr<StateNode>& current) {
                  builder.appendState(current);
              },
              [](const std::pair<std::shared_ptr<StateNode>, std::optional<char>>& connection) {
                  return connection.second == '\0';
              });

        compositeStates.push_back(builder.build());
    }

    return compositeStates;
}

std::unordered_set<char> FiniteAutomaton::extractSigmaFromRegex(const std::string& postfix) {
    std::unordered_set<char> result;
    const std::unordered_set<char> symbols = {'+', '|', '(', ')', '?', '.', '*'};
    for (auto c : postfix) {
        if (symbols.contains(c)) {
            continue;
        }
        result.insert(c);
    }
    return result;
}


static void PrintTokenHierarchy(const std::shared_ptr<RegToken>& token) {
    std::unordered_set<int> visited;
    std::stack<std::pair<std::shared_ptr<StateNode>, int>> toExplore;

    toExplore.emplace(token->startNode, 0);

    while (!toExplore.empty()) {
        auto [node, indent] = toExplore.top();
        toExplore.pop();

        if (!node || visited.contains(node->id)) continue;
        visited.insert(node->id);

        for (const auto& [otherNode, symbol] : node->connections) {
            std::string indentStr(indent * 4, ' ');
            std::cout << indentStr << node->toString()
                      << "--" << (symbol == '\0' ? 'L' : symbol)
                      << "--> " << otherNode->toString() << "\n";

            if (!visited.contains(otherNode->id)) {
                toExplore.emplace(otherNode, indent + 1);
            }
        }
    }
}
/*
*static void PrintTokenHierarchy(Token token) {
HashSet<AutomatonNode> visited = new();
Stack<(AutomatonNode, int)> toExplore = new();

toExplore.Push((token.startNode, 0));

while(toExplore.Count > 0) {
(var node, var indent)= toExplore.Pop();
visited.Add(node);

foreach(var connection in node.connections) {
var otherNode = connection.Item1;

Console.WriteLine($"{new string(' ', indent * 4)}{node}--{(connection.Item2 == null ? 'L' : connection.Item2)}-->{otherNode}");

if(visited.Contains(otherNode)) continue;
toExplore.Push((otherNode, indent + 1));
}
}
*/

FiniteAutomaton* FiniteAutomaton::buildFromRegex(const std::string& postfix) {
    sigma = extractSigmaFromRegex(postfix);

    int node_count;
    const std::shared_ptr<RegToken> automataToken = RegToken::getENFAToken(postfix, node_count);
    std::vector<std::shared_ptr<StateNode>> nodes(node_count);

    PrintTokenHierarchy(automataToken);

    DFS(automataToken->startNode,
        [&](const std::shared_ptr<StateNode>& current) {
            nodes[current->id] = current;
        },
        [](const std::pair<std::shared_ptr<StateNode>, std::optional<char>>&) {
            return true;
        });

//  std::vector<StateCluster> compositeStates = LambdaClosure(nodes);

    return nullptr;
}

// To implement (Optional)
// FiniteAutomaton* FiniteAutomaton::getMinimizedVersion() {
//     if (!isNondeterministic()) {
//         return nullptr;
//     }
//
//
//     return nullptr;
// }
