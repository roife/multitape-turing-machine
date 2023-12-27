#include "tm.h"
#include "common.h"
#include <deque>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <utility>

bool TMachineDef::Transition::match(State from,
                                    std::vector<TapeSym> read) const {
  if (this->from != from) {
    return false;
  }

  for (size_t i = 0; i < read.size(); ++i) {
    if (this->read[i] == '*' ? read[i] == blank_sym
                             : this->read[i] != read[i]) {
      return false;
    }
  }
  return true;
}

TMachineDef::State TMachineDef::StateMap::get_index(std::string name) const {
  auto it = name_to_index.find(name);
  if (it == name_to_index.end()) {
    assert(false && "state not found");
  } else {
    return it->second;
  }
}

void TMachineDef::StateMap::add_state(std::string name) {
  if (name_to_index.find(name) != name_to_index.end()) {
    assert(false && "state already exists");
  } else {
    name_to_index[name] = index_to_name.size();
    index_to_name.push_back(name);
  }
}

std::string TMachineDef::StateMap::get_name(State index) const {
  if (index >= index_to_name.size()) {
    assert(false && "state not found");
  } else {
    return index_to_name[index];
  }
}

void TMachineDef::check_input_string(const char *input, bool verbose) {
  if (input == nullptr) {
    std::cerr << "illegal input string" << std::endl;
    exit(ErrorCode::NO_INPUT_STRING);
  }

  auto input_len = strlen(input);
  for (auto i = 0; i < input_len; ++i) {
    auto c = input[i];
    if (input_alphabet.find(c) == input_alphabet.end()) {
      if (verbose) {
        std::cerr << "Input: " << input << std::endl
                  << "==================== ERR ===================="
                  << std::endl
                  << "error: Symbol \"" << c
                  << "\" in input is not defined in the set of input symbols"
                  << std::endl
                  << "Input: " << input << std::endl
                  << std::setfill(' ') << std::setw(8 + i) << '^' << std::endl
                  << "==================== END ===================="
                  << std::endl;
        exit(ErrorCode::ILLEGAL_INPUT_STRING);
      } else {
        std::cerr << "illegal input string" << std::endl;
        exit(ErrorCode::ILLEGAL_INPUT_STRING);
      }
    }
  }
}

bool TMachine::step() {
  auto range = def.transitions.equal_range(state);
  auto it = range.first;
  auto read = std::vector<TMachineDef::TapeSym>(def.tape_num);

  for (auto i = 0; i < def.tape_num; ++i) {
    read[i] = tapes[i].get_cur();
  }

  for (; it != range.second; ++it) {
    if (it->second.match(state, read)) {
      break;
    }
  }

  if (it == range.second) {
    return false;
  }

  const auto &transition = it->second;

  for (auto i = 0; i < def.tape_num; ++i) {
    if (transition.read[i] == '*') {
      if (transition.write[i] != '*') {
        tapes[i].get_cur() = transition.write[i];
      }
      // else do nothing
    } else {
      tapes[i].get_cur() = transition.write[i];
    }

    if (transition.move[i] == TMachineDef::Transition::Direction::LEFT) {
      if (tapes[i].cur == 0) {
        tapes[i].push_front(def.blank_sym);
        ++tapes[i].zero_pos;
      } else {
        --tapes[i].cur;
      }
    } else if (transition.move[i] ==
               TMachineDef::Transition::Direction::RIGHT) {
      if (tapes[i].cur == tapes[i].size() - 1) {
        tapes[i].push_back(def.blank_sym);
      }
      ++tapes[i].cur;
    }
  }

  state = transition.to;
  return true;
}

void TMachine::print_instant_state(unsigned int step_cnt) {
  auto tapes_num_str = 5 + std::to_string(def.tape_num - 1).length() + 1;

  std::cout << std::setfill(' ') << std::left << std::setw(tapes_num_str)
            << "Step"
            << ": " << step_cnt << std::endl
            << std::setw(tapes_num_str) << "State"
            << ": " << def.states_map.get_name(state) << std::endl
            << std::setw(tapes_num_str) << "Acc"
            << ": " << (pass_final_state ? "Yes" : "No") << std::endl;

  for (auto i = 0; i < def.tape_num; ++i) {
    auto beg_end = get_tape_range(tapes[i]);

    std::cout << std::left << "Index" << std::setw(tapes_num_str - 5) << i
              << ":";
    for (auto j = beg_end.first; j <= beg_end.second; ++j) {
      auto true_pos = abs((int)j - (int)tapes[i].zero_pos);
      std::cout << ' ' << true_pos;
    }
    std::cout << std::endl;

    auto spaces = 0;

    std::cout << std::left << "Tape" << std::setw(tapes_num_str - 4) << i
              << ":";
    std::cout << std::right;
    for (auto j = beg_end.first; j <= beg_end.second; ++j) {
      auto true_pos = abs((int)j - (int)tapes[i].zero_pos);
      auto spc = (true_pos > 0 ? std::to_string(true_pos - 1).length() : 1) + 1;
      std::cout << std::setw(spc);
      std::cout << tapes[i][j];

      if (j <= tapes[i].cur) {
        spaces += spc;
      }
    }
    std::cout << std::endl;

    std::cout << std::left << "Head" << std::setw(tapes_num_str - 4) << i
              << ":";
    std::cout << std::right << std::setw(spaces) << '^' << std::endl;
  }

  std::cout << "---------------------------------------------" << std::endl;
}

std::pair<size_t, size_t>
TMachine::get_tape_range(TMachine::Tape const &tape) const {
  size_t start = 0, end = tape.size() - 1;

  while (start < end && tape[start] == def.blank_sym && start < tape.cur) {
    ++start;
  }

  while (start < end && tape[end] == def.blank_sym && end > tape.cur) {
    --end;
  }

  return std::make_pair(start, end);
}

void TMachine::run(bool verbose) {
  if (verbose) {
    auto beg_end = get_tape_range(tapes[0]);
    std::string input(tapes[0].begin() + beg_end.first,
                      tapes[0].begin() + beg_end.second + 1);
    std::cout << "Input: " << input << std::endl
              << "==================== RUN ====================" << std::endl;
  }

  unsigned int step_cnt = 0;
  do {
    if (def.final_states.find(state) != def.final_states.end()) {
      pass_final_state = true;
    }

    if (verbose) {
      print_instant_state(step_cnt);
    }

    ++step_cnt;
  } while (step());

  std::string is_accepted = pass_final_state ? "ACCEPTED" : "UNACCEPTED";

  auto beg_end = get_tape_range(tapes[0]);
  std::deque<char> result(tapes[0].begin() + beg_end.first,
                          tapes[0].begin() + beg_end.second + 1);
  while (!result.empty() && result.back() == def.blank_sym) {
    result.pop_back();
  }
  while (!result.empty() && result.front() == def.blank_sym) {
    result.pop_front();
  }
  std::string result_str(result.begin(), result.end());

  if (verbose) {
    std::cout << is_accepted << std::endl
              << "Result: " << result_str << std::endl
              << "==================== END ====================" << std::endl;
  } else {
    std::cout << "(" << is_accepted << ") " << result_str << std::endl;
  }
}
