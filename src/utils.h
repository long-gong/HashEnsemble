#ifndef UTILS_H_
#define UTILS_H_

#include <string>

namespace nonstd {
inline void print_if(const std::string &msg, bool condition);
inline void println_if(const std::string &msg, bool condition);
} // nonstd


/* implementation */
namespace nonstd {

void print_if(const std::string &msg, bool condition) {
  if (condition) std::cout << msg ;
}

void println_if(const std::string &msg, bool condition) {
  if (condition) std::cout << msg << "\n";
}

} // nonstd
#endif //UTILS_H_
