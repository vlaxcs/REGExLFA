# REGEx Engine - Formal Languages and Automata Theory

> Minciunescu Vlad-Andrei / 151 <br>
> Faculty of Mathematics and Informatics <br>
> University of Bucharest

## Description
<b> A Finite Automata Engine for Regular Expression Processing </b>

- Programming language: C++20

- This project implements a complete pipeline for processing regular expressions using formal language and automata theory principles. 
- It supports converting infix regex expressions to postfix notation, constructing epsilon-NFAs via Thompson's algorithm, transforming NFAs into DFAs using subset construction, and simulating input word acceptance. 
- The application is structured around a modular class architecture and uses JSON-based configuration files for automated testing via a Tester singleton. 
- Core components include FiniteAutomaton, RegToken, and utility classes like StateNode, StateCluster, and PostfixConverter. 
- The project is built with modern C++ practices and leverages the nlohmann_json library for configuration parsing.

## Tasks

- [x] Regular expressions converter from infix to postfix form;
- [x] NFA construction using Thompson's Algorithm, from postfix form;
- [x] Conversion from NFA to DFA using subset construction;
- [x] DFA simulator
- [x] Program testing using configuration prompts stored in JSON files

## Implementation
- The program consists of test verification by executing specific tasks, so the core object of the application is <code>Tester</code> (singleton).
- Warning! The project uses an external library for JSON files parsing (<i>nlohmann_json</i>). It is included through CMakeLists.txt configuration file, which should guarantee its properly linking:
```
include(FetchContent)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

target_link_libraries(REGExLFA PRIVATE nlohmann_json::nlohmann_json)
```

## Program objects

- ### <i>Tester</i> > <b>[Header](./include/Tester.h) | [Source](./src/Tester.cpp)</b>
  The <code>tester</code> class initializes and runs user tests.
    - <code>init()</code>: Retrieves data about the existing tests from all JSON files in the <code>REGExLFA/tests</code> directory. The location of this directory is a private attribute of the <code>Tester</code> class and can be changed in the code.
    - <code>run()</code>: Accesses the retrieved data about each test (stored in an unordered_map) and compares the program's results with the expected ones. It provides the comparison result and deletes all run tests at the end.

- ### <i>PostfixConverer</i> > <b>[Header](./include/PostfixConverter.h) | [Source](./src/PostfixConverter.cpp)</b>
  The <code>PostfixConverter</code> class provides <code>getPostfix(expression)</code>, which converts an expression from infix to postfix notation.
    - <code>add_concat_symbols</code>: Adds concatenation symbols to the infix form of the regular expression, according to the validation expression rules.
    - <code>to_postfix</code>: After applying the concatenation symbols, the postfix form is obtained from the infix form, calculated using the Shunting-Yard algorithm.

- ### <i>FiniteAutomaton</i> > <b>[Header](./include/FiniteAutomaton.h) | [Source](./src/FiniteAutomaton.cpp)</b>
  The `FiniteAutomaton` class encapsulates the logic and structure of a finite automaton, supporting construction from configuration files or postfix regular expressions, and enabling conversion to an NFA or DFA.

  ---

  - ### Initialization

    A `FiniteAutomaton` object can be initialized in two ways:

    - **From a configuration file**:
      - Defines:
        - Alphabet (`Sigma`)
        - States (with optional start/final indicators)
        - Transitions
    - **From a regular expression in postfix form**:
      - Automatically builds an NFA using the regex.

  ---

  - ### Constructor

    ```cpp
    FiniteAutomaton(const std::string &file);
    ```

    - Parses the file and sets up the automaton:
      - Calls `setSigma`, `setStates`, `setTransitions`, and `setStartState`.

  ---

  - ### Attributes

    - `sigma`: Set of characters representing the input alphabet.
    - `states`: Vector of all state instances.
    - `stateMap`: Maps state names to shared pointers.
    - `startState`: Pointer to the initial state.

  ---

  - ### Key Methods

    ### Public:

    - `bool process(const std::string& word) const`:

      - Checks if a given string is accepted by the automaton.

    - `void setSigma(const std::unordered_set<char> &sigma)`:

      - Sets the alphabet explicitly.

    - `std::unordered_set<char> getSigma()`:

      - Returns the alphabet.

    - `static FiniteAutomaton* buildFromRegex(const std::string& postfix)`:

      - Builds an NFA from a regular expression in postfix notation.

    ### Protected:

    - `void setSigma(const std::vector<std::string> &sigma_)`:

      - Extracts and sets the alphabet from configuration data.

    - `void setStates(const std::vector<std::string> &stateLines)`:

      - Initializes states and identifies start/final states.

    - `void setTransitions(const std::vector<std::string> &transitions)`:

      - Establishes transitions between states.

    - `void setStartState()`:

      - Sets the start state based on initial state marker.

    - `bool inSigma(const char &symbol) const`:

      - Checks if a character belongs to the alphabet.

    - `bool isNondeterministic() const`:

      - Returns true if any state has multiple transitions for a symbol.

  ---

  - ### Static Utilities

    - ### DFS

      ```cpp
      static void DFS(
          const std::shared_ptr<StateNode>& origin,
          const std::function<void(std::shared_ptr<StateNode>)>& action,
          const std::function<bool(const std::pair<std::shared_ptr<StateNode>, std::optional<char>>&)>& validate);
      ```

      - Performs a depth-first search over the state graph.
      - Applies a user-defined `action` to each visited node.
      - Uses `validate` to control which transitions are followed.

    - ### LambdaScope

      ```cpp
      static std::vector<StateCluster> LambdaScope(const std::vector<std::shared_ptr<StateNode>>& nodes);
      ```

      - Computes epsilon (lambda) closures for all states.
      - Each `StateCluster` represents the full epsilon-closure of a state.

    - ### extractSigmaFromRegex

      ```cpp
      static std::unordered_set<char> extractSigmaFromRegex(const std::string& postfix);
      ```

      - Extracts valid alphabet characters from a postfix regex.
      - Ignores regex operators like `+`, `|`, `*`, `.` etc.

      ---

    - ### buildFromRegex

      ```cpp
      FiniteAutomaton* FiniteAutomaton::buildFromRegex(const std::string &postfix);
      ```

      ### Purpose:

      Builds a finite automaton (FA) — specifically an epsilon-NFA — from a postfix regular expression.

      ### Steps:
      1. **Initialization**:
        - Creates a new `FiniteAutomaton` object.
        - Extracts and sets the alphabet (`Sigma`) from the postfix regex.

      2. **Postfix Tokenization**:
        - Calls `RegToken::getENFAToken` to generate a linked structure of state transitions based on the postfix regex.

      3. **Graph Traversal**:
        - Applies depth-first search (`DFS`) from the start node to collect all state nodes in an indexed vector.

      4. **Lambda Closures**:
        - Computes epsilon closures (lambda transitions) using `LambdaScope`.

      5. **Expansion Table**:
        - For each state and each symbol in the alphabet, computes where the automaton would go (post-closure) — this forms a DFA-style transition table.

      6. **State Mapping**:
        - Assigns integer indices to unique state clusters (i.e., sets of NFA states).
        - Builds a transition map (`transition_rules`) between these indices.

      7. **FA State Creation**:
        - Creates FA states named `q0`, `q1`, ..., with proper `initial` and `final` flags.
        - Final states are identified by checking if they contain the original NFA’s `endNode`.

      8. **Transition Linking**:
        - Links the created states using the populated `transition_rules`.

      9. **Returns**:
        - The constructed `FiniteAutomaton` instance.

      ---

    - ### process

      ```cpp
      bool FiniteAutomaton::process(const std::string& word) const;
      ```

        - #### Purpose: Checks whether the finite automaton accepts a given input word.

        - #### Steps:
        1. Starts from `startState`.
        2. Iterates through each symbol in the input `word`.
        3. For each symbol:
          - Attempts to find a transition from the current state using the symbol.
          - If no transition is found, returns `false`.
          - Otherwise, moves to the next state.
        4. After processing all symbols:
          - Returns `true` if the final state is marked as `final`, `false` otherwise.

      #### Note:
      This method assumes the FA is deterministic (DFA), as it does not handle multiple possible transitions or epsilon transitions.

    - ### Operator Overload

      #### `operator<<`

      ```cpp
      friend std::ostream& operator<<(std::ostream& os, const FiniteAutomaton& fa);
      ```

      - Prints a formatted representation of the automaton:
        - Type (NFA or DFA)
        - Alphabet
        - States and final states
        - Start state
        - Transition map for each state

    ---

    #### Regex to FA (Overview)

      - Converts a postfix regex into an epsilon-NFA (ENFA).
      - Constructs state graph using `RegToken::getENFAToken()`.
      - Applies `LambdaScope` to compute closures.
      - Builds transition table.
      - Assigns names like `q0`, `q1`, etc., to new states.
      - Determines final states based on reachability of the regex's terminal node.

      ---

    #### Notes

    - Designed for extensibility and clarity.
    - Uses shared pointers for safe memory management.
    - Includes validation and warning mechanisms for input consistency.
    - DFA minimization is planned but not yet implemented.

- ## <i>RegToken</i> > <b>[Header](./include/RegToken.h) | [Source](./src/RegToken.cpp)</b>

  The `RegToken` class is a helper structure used to construct ε-NFAs (epsilon non-deterministic finite automatons) from postfix regular expressions. It encapsulates a pair of start and end nodes for a regex fragment and provides logic to combine and transform these fragments into complex automata structures.

  - ### Attributes

    - `startNode`: A shared pointer to the starting `StateNode` of the regex token.
    - `endNode`: A shared pointer to the ending `StateNode` of the regex token.

    ---

  - ### Constructors

    - `RegToken()`  
      Default constructor.

    - `RegToken(std::shared_ptr<StateNode> startNode, std::shared_ptr<StateNode> endNode)`  
      Directly initializes with specific start and end nodes.

    - `RegToken(char symbol, int &i)`  
      Builds a basic automaton for a single character. It creates two new nodes and connects them using the given symbol.

    - `RegToken(int &i, const std::shared_ptr<RegToken> &lhs, char operation = 'x', const std::shared_ptr<RegToken>& rhs = nullptr)`  
      Combines existing tokens using regex operations (`.`, `|`, `*`, `+`, `?`).

  ---

  - ### Static Methods

    #### `getENFAToken`

    ```cpp
    static std::shared_ptr<RegToken> getENFAToken(const std::string& postfix, int& nodesCount);
    ```

    - Converts a postfix regular expression into an ε-NFA.
    - Builds tokens for each character and merges them according to postfix rules.
    - Throws runtime error if the expression is malformed.

    ---

  - ### Internal Helpers

    - `isOperator(char c)` – checks if `c` is a regex operator.
    - `isBinary(char c)` – checks if `c` is a binary regex operator (`.` or `|`).

---

- #### Regex Operations Explained

  - `.` (Concatenation): Joins `lhs` and `rhs` by linking `lhs.endNode` to `rhs.startNode`.
  - `|` (Union): Creates a new start and end node with epsilon transitions to/from both `lhs` and `rhs`.
  - `*` (Kleene Star): Adds looping epsilon transitions for zero or more repetitions.
  - `+` (One or more): Similar to `*`, but requires at least one pass.
  - `?` (Optional): Allows either a single pass or skipping the token entirely.

---

- #### Notes

  - The class assumes valid postfix expressions.
  - Uses `StateNode` and epsilon transitions (symbol `' '`) to connect sub-automata.


## State Classes Documentation

- ## <i>State</i> > <b>[Header](./include/State.h)</b>
  The `State` struct models a node (or state) in a finite automaton. It encapsulates the identity and behavior of a single state, including its role (initial/final) and its outgoing transitions.

  - ### Attributes:
    - `std::string name`  
      A unique identifier for the state (e.g., "q0", "q1").

    - `bool initial = false`  
      Indicates whether this state is the initial (start) state of the automaton.

    - `bool final = false`  
      Marks the state as final (accepting) if set to true.

    - `std::unordered_multimap<char, std::shared_ptr<State>> transitions`  
      Stores the outgoing transitions from this state.
      - The key is a `char` representing the input symbol for the transition.
      - The value is a `shared_ptr` to the target `State` object.
      - `unordered_multimap` allows multiple transitions for the same input symbol (needed for NFAs).

  - ### Properties:
    - `std::shared_ptr<State>` ensures safe memory management when states are shared across structures like graphs.
    - This struct is often used in building DFAs and NFAs, where transitions and state roles (start/accepting) are fundamental.
    - The use of `unordered_multimap` is especially relevant for non-deterministic automata, where a single input symbol can lead to multiple possible next states.


- ## <i>StateNode</i> > <b>[Header](./include/StateNode.h)</b>

  The `StateNode` class represents a state (node) in a finite automaton, primarily used during the construction of epsilon-NFAs (ENFAs).

  - ### Attributes:
    - `int id` – A unique numeric identifier for the node.
    - `std::vector<std::pair<std::shared_ptr<StateNode>, char>> connections` – A list of transitions from this node to others. Each pair includes a target node and a transition symbol (`'\0'` represents an epsilon transition).

  - ### Methods:
    - `explicit StateNode(int id)` – Constructor that assigns the node ID.
    - `void appendConnection(std::shared_ptr<StateNode> node, char connection = '\0')` – Adds a transition to the given node using the specified symbol.
    - `bool operator<(const StateNode& node) const` – Enables comparison of nodes by ID (for use in sets or maps).
    - `std::string toString() const` – Returns a formatted string representation of the node, e.g., `(3)`.

- ## <i>StateCluster</i> > <b>[Header](./include/StateCluster.h) | [Source](./src/StateCluster.cpp)</b>

  The `StateCluster` class represents a set of states used to model state closures or compound states during NFA → DFA conversion.

  - ### Key Methods:
    - `StateCluster stepWith(char symbol, const std::vector<std::shared_ptr<StateNode>>& id_map) const`  
      - Determines the set of nodes reachable via a transition labeled `symbol` from any node in the current cluster.

    - `std::string toString() const`  
      - Returns a string representation of the cluster in the format `{(1),(2),...}`.

- ## <i>StateAssembler</i> > <b>[Header](./include/StateAssembler.h) | [Source](./src/StateAssembler.cpp)</b>

  The `StateAssembler` class is a utility used to incrementally build a `StateCluster`.

  - ### Attributes:
    - `std::vector<std::shared_ptr<StateNode>> states` – A collection of nodes that will be part of the resulting cluster.

  - ### Methods:
    - `void appendState(std::shared_ptr<StateNode> state)` – Adds a single node to the cluster.
    - `void appendState(const StateCluster& nodes)` – Appends all nodes from an existing `StateCluster`.
    - `StateCluster build() const` – Constructs and returns a `StateCluster` from the accumulated nodes.