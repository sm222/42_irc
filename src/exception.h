#pragma once

#include <iostream> 

class AnyExcept : public std::exception {
private:
    std::string reason;
public:
    AnyExcept(const std::string& msg);
    const char* what() const throw();
};