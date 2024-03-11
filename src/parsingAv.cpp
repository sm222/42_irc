#include "parsingAv.hpp"

static  bool lookNumber(std::string args) {

}

short parsingAv(int ac, char **av, valueAv& value) {

  if (ac < 3 || ac > 3) {
    std::cout << av[0] << "<port> <password>" << std::endl;
    return 0;
  }
  value.tmp = av[1];
  if (value.tmp.length() > 5)
  return 1;
}