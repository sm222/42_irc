#include "signal.h"
#include <csignal>

static void signalReceived(int sig) {
    (void)sig;
    *signalStop() = true;
}

bool*   signalStop() {
    static bool T;
    return &T;
}

void initSignal() {
    signal(SIGINT, signalReceived);  
    signal(SIGTERM, signalReceived);
    signal(SIGQUIT, signalReceived);
}
