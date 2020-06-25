#include "hashensemble.h"
#include <iostream>

using namespace nonstd;

void dummy() {
  std::cout << "Dummy ... ";
  HashEnsemble h;
  std::cout << "Passed\n";
}

uint64_t uint64_example() {
  std::cout << "64-bit Integer ... ";
  HashEnsemble h;
  uint64_t key = 1009u;
  auto val = h(key);
  std::cout << "Passed\n";
  return val;
}

uint64_t string_example() {
  std::cout << "string ... ";
  HashEnsemble h;
  std::string key = "1009u";
  auto val = h(key);
  std::cout << "Passed\n";
  return val;
}

int main(int argc, char** argv) {
  dummy();
  uint64_example();
  string_example();
  return 0;
}


