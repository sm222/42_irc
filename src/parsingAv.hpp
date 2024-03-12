#pragma once

#include <stdint.h>
#include <string>
#include <iostream>
#include <cstring>


struct valueAv {
  std::string     tmp;
  uint16_t        port;
  std::string     pass;
};

short parsingAv(int ac, char **av, valueAv& value);