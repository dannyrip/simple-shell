#include <sys/types.h>

extern pid_t childpid;

void sigint_handler(int);

void sigtstp_handler(int);