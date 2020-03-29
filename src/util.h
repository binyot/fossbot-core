#ifndef FOSSBOT_CORE_UTIL_H
#define FOSSBOT_CORE_UTIL_H

#include <algorithm>
#include <string>

namespace nonstd {

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

inline std::string trim(const std::string &s)
{
  const auto isspace = [](int c){return std::isspace(c);};
  auto wsfront = std::find_if_not(begin(s), end(s), isspace);
  return std::string(wsfront, std::find_if_not(rbegin(s), std::string::const_reverse_iterator(wsfront), isspace).base());
}

}


#endif//FOSSBOT_CORE_UTIL_H
