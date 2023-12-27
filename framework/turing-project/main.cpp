#include "common.h"
#include "option.h"
#include "parser.h"
#include "tm.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>

char *read_tm_file(const char *tm_filename) {
  char *tm_file = nullptr;
  if (tm_filename == nullptr) {
    std::cerr << "illegal tm file" << std::endl;
    exit(ErrorCode::NO_TM_FILE);
  } else {
    int fd = open(tm_filename, O_RDONLY);
    if (fd < 0) {
      std::cerr << "illegal tm file" << std::endl;
      exit(ErrorCode::TM_FILE_OPEN_FAILED);
    }

    struct stat *st = (struct stat *)malloc(sizeof(struct stat));
    fstat(fd, st);
    tm_file = (char *)mmap(nullptr, st->st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  }
  return tm_file;
  // TODO: free tm_file
}

int main(int argc, char **argv) {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  const auto opts = Option(argc, argv);
  if (opts.help) {
    std::cout << "usage: " << argv[0]
              << " [-v|--verbose] [-h|--help] <tm> <input>" << std::endl;
    return 0;
  }

  auto tm_file = read_tm_file(opts.tm_filename);
  TMachineDef tm_def = TMParse(tm_file).parse();

  tm_def.check_input_string(opts.input, opts.verbose);
  TMachine tm(tm_def, opts.input);
  tm.run(opts.verbose);
}
