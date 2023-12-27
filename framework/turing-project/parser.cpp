#include "parser.h"
#include "tm.h"
#include <iostream>
#include <string>
#define parsing_assert(cond)                                                   \
  if (!(cond)) {                                                               \
    std::cerr << "syntax error" << std::endl;                                  \
    exit(ErrorCode::PARSING_ERROR);                                            \
  }

void TMParse::skip() {
  // skip spaces and comments
  while (*cur_ptr) {
    if (*cur_ptr == ';') {
      while (*cur_ptr && *cur_ptr != '\n') {
        ++cur_ptr;
      }
    } else if (isspace(*cur_ptr)) {
      ++cur_ptr;
    } else {
      break;
    }
  }
}

void TMParse::expect_str(const char *str) {
  for (auto ptr = str; *ptr; ++ptr) {
    expect_and_next(*ptr);
  }
}

void TMParse::expect_and_next(char c) {
  if (!*cur_ptr || *cur_ptr != c) {
    parsing_assert(false);
  }
  ++cur_ptr;
}

TMachineDef::TapeSym TMParse::parse_blank_sym() {
  skip();
  expect_str("#B = ");
  return *cur_ptr++;
}

std::string TMParse::parse_state_name() {
  auto beg = cur_ptr;
  while (*cur_ptr && is_state_char(*cur_ptr)) {
    ++cur_ptr;
  }
  parsing_assert(beg != cur_ptr);
  return std::string(beg, cur_ptr);
}

TMachineDef::State
TMParse::parse_init_state(TMachineDef::StateMap const &state_map) {
  skip();
  expect_str("#q0 = ");
  return state_map.get_index(parse_state_name());
}

std::unordered_set<TMachineDef::TapeSym> TMParse::parse_tape_alphabet() {
  skip();
  expect_str("#G = {");

  static std::unordered_set<TMachineDef::TapeSym> INVALID_SYMS = {
      ' ', ',', ';', '{', '}', '*'};
  std::unordered_set<TMachineDef::TapeSym> tape_alphabet;
  while (*cur_ptr != '}') {
    parsing_assert(isprint(*cur_ptr) &&
                   INVALID_SYMS.find(*cur_ptr) == INVALID_SYMS.end() &&
                   "invalid input symbol");
    tape_alphabet.insert(*cur_ptr++);
    parsing_assert((*cur_ptr == ',' || *cur_ptr == '}'));
    if (*cur_ptr == ',') {
      ++cur_ptr;
    }
  }

  expect_str("}");

  return tape_alphabet;
}

std::unordered_set<TMachineDef::TapeSym> TMParse::parse_input_alphabet() {
  skip();
  expect_str("#S = {");

  static std::unordered_set<TMachineDef::TapeSym> INVALID_SYMS = {
      ' ', ',', ';', '{', '}', '*', '_'};
  std::unordered_set<TMachineDef::TapeSym> input_alphabet;
  while (*cur_ptr != '}') {
    parsing_assert(isprint(*cur_ptr) &&
                   INVALID_SYMS.find(*cur_ptr) == INVALID_SYMS.end() &&
                   "invalid input symbol");
    input_alphabet.insert(*cur_ptr++);
    parsing_assert((*cur_ptr == ',' || *cur_ptr == '}'));
    if (*cur_ptr == ',') {
      ++cur_ptr;
    }
  }

  expect_str("}");

  return input_alphabet;
}

std::vector<std::string> TMParse::parse_states() {
  skip();
  expect_str("#Q = {");

  std::vector<std::string> states;
  while (*cur_ptr != '}') {
    auto state_name = parse_state_name();
    parsing_assert((*cur_ptr == ',' || *cur_ptr == '}'));

    states.push_back(state_name);
    if (*cur_ptr == ',') {
      ++cur_ptr;
    }
  }

  expect_str("}");

  return states;
}

std::unordered_set<TMachineDef::State>
TMParse::parse_final_states(TMachineDef::StateMap const &state_map) {
  skip();
  expect_str("#F = {");

  std::unordered_set<TMachineDef::State> final_states;
  while (*cur_ptr != '}') {
    auto state_name = parse_state_name();
    parsing_assert((*cur_ptr == ',' || *cur_ptr == '}'));

    final_states.insert(state_map.get_index(state_name));
    if (*cur_ptr == ',') {
      ++cur_ptr;
    }
  }

  expect_str("}");

  return final_states;
}

size_t TMParse::parse_tape_num() {
  skip();
  expect_str("#N = ");

  size_t tape_num = 0;
  while (isdigit(*cur_ptr)) {
    tape_num = tape_num * 10 + (*cur_ptr - '0');
    ++cur_ptr;
  }
  parsing_assert(tape_num > 0);

  return tape_num;
}

std::vector<TMachineDef::TapeSym> TMParse::parse_tape_symbols(
    size_t tape_num,
    std::unordered_set<TMachineDef::TapeSym> const &tape_alphabet) {
  std::vector<TMachineDef::TapeSym> tape_symbols;
  for (size_t i = 0; i < tape_num; ++i) {
    auto tape_sym = *cur_ptr;
    parsing_assert((tape_alphabet.find(tape_sym) != tape_alphabet.end() ||
                   tape_sym == '*') &&
                   "invalid tape symbol");
    tape_symbols.push_back(tape_sym);
    ++cur_ptr;
  }
  return tape_symbols;
}

std::vector<TMachineDef::Transition::Direction>
TMParse::parse_tape_directions(size_t tape_num) {
  std::vector<TMachineDef::Transition::Direction> tape_directions;
  for (size_t i = 0; i < tape_num; ++i) {
    auto tape_direction = *cur_ptr++;
    parsing_assert((tape_direction == 'l' || tape_direction == 'r' ||
                    tape_direction == '*'));
    tape_directions.push_back(
        tape_direction == 'l'   ? TMachineDef::Transition::Direction::LEFT
        : tape_direction == 'r' ? TMachineDef::Transition::Direction::RIGHT
                                : TMachineDef::Transition::Direction::STAY);
  }
  return tape_directions;
}

TMachineDef::Transition TMParse::parse_transition(
    TMachineDef::StateMap const &state_map, size_t tape_num,
    std::unordered_set<TMachineDef::TapeSym> const &tape_alphabet,
    TMachineDef::TapeSym blank_sym) {
  skip();

  auto from = state_map.get_index(parse_state_name());
  expect_and_next(' ');

  auto input_sym = parse_tape_symbols(tape_num, tape_alphabet);
  expect_and_next(' ');

  auto output_sym = parse_tape_symbols(tape_num, tape_alphabet);
  expect_and_next(' ');

  auto directions = parse_tape_directions(tape_num);
  expect_and_next(' ');

  auto to = state_map.get_index(parse_state_name());

  return TMachineDef::Transition(from, input_sym, to, output_sym, directions,
                                 blank_sym);
}

std::unordered_multimap<TMachineDef::State, TMachineDef::Transition>
TMParse::parse_transitions(
    TMachineDef::StateMap const &state_map, size_t tape_num,
    std::unordered_set<TMachineDef::TapeSym> const &tape_alphabet,
    TMachineDef::TapeSym blank_sym) {
  std::unordered_multimap<TMachineDef::State, TMachineDef::Transition>
      transitions;
  while (*cur_ptr) {
    auto transition =
        parse_transition(state_map, tape_num, tape_alphabet, blank_sym);
    transitions.insert(std::make_pair(transition.from, transition));
    skip();
  }

  return transitions;
}

TMachineDef TMParse::parse() {
  parsing_assert(cur_ptr == input.c_str());
  const auto state_map = TMachineDef::StateMap(std::move(parse_states()));
  const auto input_alphabet = parse_input_alphabet();
  const auto tape_alphabet = parse_tape_alphabet();
  const auto init_state = parse_init_state(state_map);
  const auto blank_sym = parse_blank_sym();
  parsing_assert(tape_alphabet.find(blank_sym) != tape_alphabet.end() &&
                 "blank symbol must be in tape alphabet");
  const auto final_states = parse_final_states(state_map);
  const auto tape_num = parse_tape_num();
  const auto transitions =
      parse_transitions(state_map, tape_num, tape_alphabet, blank_sym);

  return TMachineDef(std::move(state_map), init_state, std::move(final_states),
                     blank_sym, std::move(input_alphabet),
                     std::move(transitions), tape_num);
}
