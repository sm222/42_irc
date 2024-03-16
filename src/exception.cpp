#include "exception.h"

AnyExcept::~AnyExcept() throw(){}
AnyExcept::AnyExcept(const std::string& msg) : reason(msg) {}
const char* AnyExcept::what() const throw() { return reason.c_str(); }