#pragma once

#include <cassert>

#define unreachable()                           \
    do {                                        \
        assert(false && "unreachable");         \
    } while (0)

#define todo()                                  \
    do {                                        \
        assert(false && "todo");                \
    } while (0)

#define unimplemented()                         \
    do {                                        \
        assert(false && "unimplemented");       \
    } while (0)


enum ErrorCode {
  NO_TM_FILE = 1,
  NO_INPUT_STRING,
  ILLEGAL_INPUT_STRING,
  TM_FILE_OPEN_FAILED,
  UNKNOWN_OPTION,
  PARSING_ERROR,
};
