#pragma once

#include "common.h"
#include <__config>
#include <cassert>
#include <deque>
#include <sys/_types/_size_t.h>
#include <unordered_set>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

struct TMachineDef {
    using State = size_t;
    class StateMap {
        std::unordered_map<std::string, State> name_to_index;
        std::vector<std::string> index_to_name;

    public:
        State get_index(std::string name) const;

        std::string get_name(State index) const;

        void add_state(std::string name);

        StateMap(std::vector<std::string> &&states) {
            for (size_t i = 0; i < states.size(); ++i) {
                add_state(states[i]);
            }
        }
    };

    const StateMap states_map;
    State init_state;
    const std::unordered_set<State> final_states;

    using TapeSym = char;
    const TapeSym blank_sym;
    const std::unordered_set<TapeSym> input_alphabet;
    const size_t tape_num;

    struct Transition {
        State from;
        std::vector<TapeSym> read;
        State to;
        std::vector<TapeSym> write;
        TapeSym blank_sym;

        enum Direction { LEFT, RIGHT, STAY };
        std::vector<Direction> move;

    Transition(State from, std::vector<TapeSym> read, State to,
               std::vector<TapeSym> write, std::vector<Direction> move,
               TapeSym blank_sym)
        : from(from), read(read), to(to), write(write), move(move), blank_sym(blank_sym) {
            assert(read.size() == write.size());
            assert(read.size() == move.size());
        }

        bool match(State from, std::vector<TapeSym> read) const;
    };

    const std::unordered_multimap<State, Transition> transitions;

    TMachineDef(const StateMap &&states_map, State init_state,
                const std::unordered_set<State> &&final_states,
                const TapeSym blank_sym,
                const std::unordered_set<TapeSym> &&input_alphabet,
                const std::unordered_multimap<State, Transition> &&transitions,
                const size_t tape_num)
        : states_map(std::move(states_map)), init_state(init_state),
          final_states(std::move(final_states)), blank_sym(blank_sym),
          input_alphabet(std::move(input_alphabet)),
          transitions(std::move(transitions)), tape_num(tape_num) {}

    void check_input_string(const char *input, bool verbose);
};

struct TMachine {
    TMachineDef const &def;

    TMachineDef::State state;

    struct Tape {
        std::deque<TMachineDef::TapeSym> tape;
        size_t cur;
        size_t zero_pos;

        TMachineDef::TapeSym &get_cur() {
            return tape[cur];
        }

        const TMachineDef::TapeSym &operator[](size_t index) const {
            assert(index < tape.size());
            return tape[index];
        }

        size_t size() const { return tape.size(); }

        void push_front(TMachineDef::TapeSym sym) { tape.push_front(sym); }

        void push_back(TMachineDef::TapeSym sym) { tape.push_back(sym); }

        decltype(tape)::iterator begin() { return tape.begin(); }

        decltype(tape)::iterator end() { return tape.end(); }

        Tape(size_t cur, TMachineDef::TapeSym blank_sym)
            : tape(1, blank_sym), cur(cur), zero_pos(0) {}
    };
    std::vector<Tape> tapes;

    bool pass_final_state;

    bool step();

    std::pair<size_t, size_t> get_tape_range(Tape const &tape) const;

    void print_instant_state(unsigned int step_cnt);

    void run(bool verbose);

    TMachine(TMachineDef const &def, const char *input)
        : def(def), tapes(def.tape_num, Tape(0, def.blank_sym)),
          pass_final_state(false), state(def.init_state) {
        auto input_len = strlen(input);
        tapes[0].tape.resize(input_len);
        for (size_t i = 0; i < input_len; ++i) {
            tapes[0].tape[i] = input[i];
        }
    }
};
