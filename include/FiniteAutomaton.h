#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <format>
#include <functional>
#include <optional>
#include <State.h>

#include "StateNode.h"

class FiniteAutomaton {
protected:
    std::unordered_set<char> sigma;
    std::vector<std::shared_ptr<State> > states;
    std::unordered_map<std::string, std::shared_ptr<State> > stateMap;
    std::shared_ptr<State> startState = nullptr;

    bool inSigma(const char &symbol) const;

    void setSigma(std::vector<std::string> const &sigma_);

    void setStates(const std::vector<std::string> &stateLines);

    void setTransitions(std::vector<std::string> const &transitions);

    void setStartState();

    explicit FiniteAutomaton(const std::string &file);

    //FiniteAutomaton* getMinimizedVersion();

    static std::unordered_set<char> extractSigmaFromRegex(const std::string& postfix);

public:
    FiniteAutomaton() = default;

    bool isNondeterministic();

    static void DFS(
    std::shared_ptr<StateNode> origin,
         const std::function<void(std::shared_ptr<StateNode>)>& action,
         const std::function<bool(const std::pair<std::shared_ptr<StateNode>, std::optional<char>>&)>& validate);

    std::vector<StateCluster> LambdaScope(const std::vector<std::shared_ptr<StateNode>>& nodes);
    //std::vector<StateCluster> LambdaScope(std::vector<StateNode>);
    FiniteAutomaton* buildFromRegex(const std::string& postfix);


    ~FiniteAutomaton() = default;
};

#include "StateNode.h"

// class RegToken {
// private:
//     StateNode start, end;
//
// public:
//     RegToken(const StateNode &_start, const StateNode &_end): start(_start), end(_end) {};
//     RegToken(char c, int id) : start(new StateNode(id++)), end(new StateNode(id)) {};
//
//
// };