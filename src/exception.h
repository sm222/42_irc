#pragma once

#include <iostream> 

class AnyExcept : public std::exception {
private:
    std::string reason;
public:
    virtual ~AnyExcept() throw();
    AnyExcept(const std::string& msg);
    const char* what() const throw();
};
