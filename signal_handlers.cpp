#include <iostream>
#include <signal.h>
#include "signal_handlers.h"

void sigint_handler(int signo) {

    if (childpid) { // if there is a child proccess currently running
        if (kill(childpid, SIGINT) != 0) {  // send the signal to the child
            perror("kill");
        }
        childpid = 0;
    }
    else {
        std::cout << std::endl; 
    }
}

void sigtstp_handler(int signo) {

    if (childpid) { // if there is a child proccess currently running
        if (kill(childpid, SIGTSTP) != 0) { // send the signal to the child
            perror("kill");
        }
        childpid = 0;
    }
    else {
        std::cout << std::endl; 
    }
}