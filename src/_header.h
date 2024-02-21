#pragma once

// Socket
#include <sys/socket.h>     // socket, bind, listen, connect
#include <netdb.h>          // getaddrinfo, freeaddrinfo, struct addrinfo

// Basic Stuff
#include <iostream>         // cout
#include <unistd.h>         // close
#include <string.h>         // memset

// Others
#include "exception.h"
