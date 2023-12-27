#include "tm.h"
#include <cstdio>
#include <string>

class TMParse {
  const std::string input;
  const char *cur_ptr;

  void skip();

  void expect_and_next(char c);

  void expect_str(const char *str);

  std::string parse_state_name();

  bool is_state_char(char c) { return isalnum(c) || c == '_'; }

  std::vector<std::string> parse_states();

  std::unordered_set<TMachineDef::TapeSym> parse_input_alphabet();

  std::unordered_set<TMachineDef::TapeSym> parse_tape_alphabet();

  TMachineDef::State parse_init_state(TMachineDef::StateMap const &state_map);

  TMachineDef::TapeSym parse_blank_sym();

  size_t parse_tape_num();

  std::unordered_set<TMachineDef::State>
  parse_final_states(TMachineDef::StateMap const &state_map);

  std::vector<TMachineDef::TapeSym> parse_tape_symbols(
      size_t tape_num,
      std::unordered_set<TMachineDef::TapeSym> const &tape_alphabet);
  std::vector<TMachineDef::Transition::Direction>
  parse_tape_directions(size_t tape_num);
  TMachineDef::Transition parse_transition(
      TMachineDef::StateMap const &state_map, size_t tape_num,
      std::unordered_set<TMachineDef::TapeSym> const &tape_alphabet,
      TMachineDef::TapeSym blank_sym);
  std::unordered_multimap<TMachineDef::State, TMachineDef::Transition>
  parse_transitions(
      TMachineDef::StateMap const &state_map, size_t tape_num,
      std::unordered_set<TMachineDef::TapeSym> const &tape_alphabet,
      TMachineDef::TapeSym blank_sym);

public:
  TMParse(char *input) : input(input), cur_ptr(this->input.c_str()) {}

  TMachineDef parse();
};
