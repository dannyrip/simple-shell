#include <iostream>
#include <signal.h>
#include <sys/wait.h>
#include "execute.h"
#include "parsing.h"
#include "signal_handlers.h"
using namespace std;

pid_t childpid = 0;

int main(void) {

    string input;
    vector<string> parsed;
    unordered_map<string, string> alias;
    queue<string> history;

    static struct sigaction act_int;    // signal handlers for signals SIGINT and SIGTSTP
    static struct sigaction act_tstp;
    act_int.sa_handler = sigint_handler;
    act_tstp.sa_handler = sigtstp_handler;
    act_int.sa_flags = SA_RESTART;
    act_tstp.sa_flags = SA_RESTART;
    sigfillset(&(act_int.sa_mask));
    sigfillset(&(act_tstp.sa_mask));

    sigaction(SIGINT, &act_int, NULL);
    sigaction(SIGTSTP, &act_tstp, NULL);

    while (1) {
        childpid = 0;   // global var is set to zero
        waitpid(-1, NULL, WNOHANG);     // wait for zombie proccesses that run in the background
        cout << "in-mysh-now:> ";   // get the user's input
        getline(cin, input);    // read the user's input

        if (input == "") {  //no input
            continue;;
        }

        if (input == "exit") {  //exit
            break;
        }

        if (input == "myhistory") { // if user asks for history
            print_history(history);
            continue;
        }

        parsed = tokenize(input); // parse the input

        if (parsed[0] == "myhistory") { // if input is a request for a command from history
            int pos = stoi(parsed[1]);
            input = get_history_element(history, pos);
            parsed = tokenize(input);
        }

        history_mod(history, input);    // modify history for last 20 commands
        
        execute(parsed, alias); // execute the parsed command
    }
    
    return 0;
}