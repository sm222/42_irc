#include "parsingAv.hpp"

static  bool lookNumber(std::string args) {
    for (size_t i = 0; i < args.size(); i++) {
        if (!std::strchr("0123456789", args[i]))
            return false;
    }
    return true;
}

short parsingAv(int ac, char **av, valueAv& value) {
    if (ac < 3 || ac > 3) {
        std::cout << av[0] << " <port> <password>" << std::endl;
        return 0;
    }
    value.tmp = av[1];
    if (value.tmp.length() > 5 || !lookNumber(value.tmp)) {
        std::cout << "Error :\n" << value.tmp << " is not a valid port" << std::endl;
        return 0;
    }
    try {
        value.port = std::stod(value.tmp);
    }
    catch(const std::exception& e) {
        std::cout << "stod trow" << std::endl;
        return 0;
    }
    if (av[2][0] == '\0') {
        std::cout << "Error:\nneed a password" << std::endl; 
        return 0;
    }
    value.pass = av[2];
    return 1;
}