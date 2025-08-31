#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdlib>
#include <cstring>
#include "execute.h"
#include "parsing.h"
using namespace std;

#define READ 0
#define WRITE 1

void check_for_alias_mod(std::vector<std::string>&, std::unordered_map<std::string, std::string>&);

// execute the parsed input
void execute(vector<string> parsed, unordered_map<string, string>& alias) {

    pid_t pid;
    vector<pid_t> children;
    int inputfd = -1;
    int outputfd = -1;
    int pipefd[2]; pipefd[0] = -1; pipefd[1] = -1;
    int oldpipe = -1;

    while (parsed.size() > 0) { // while there is a command to execute
        check_for_alias_mod(parsed, alias);
        replace_alias(parsed, alias);

        int argsnum = count_args(parsed);   // count how many arguments are in the command
        char** argsca = new char*[argsnum+1];   // allocate an array of pointers to char, size of arguments+1
        argsca[argsnum] = NULL; // make the last pointer NULL
        int i=0;
        while((parsed.size() > 0) && (parsed.front() != "|") && (parsed.front() != ";") && (parsed.front() != "&")) {   // while the first item of the vector parsed belongs to the current command
            if (parsed.front() == "<") {    // if the string is for input redirection
                parsed.erase(parsed.begin());
                char* inputf = stocp(parsed.front());
                if ((inputfd = open(inputf, O_RDONLY)) == -1) { // open the file to the inputfd var
                    perror("open file");
                    exit(1);
                }
                parsed.erase(parsed.begin());
                continue;
            }
            if (parsed.front() == ">") {    // if the string is for output redirection
                parsed.erase(parsed.begin());
                char* outputf = stocp(parsed.front());
                if ((outputfd = open(outputf, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) { // open the file to the output var
                    perror("open file");
                    exit(1);
                }
                parsed.erase(parsed.begin());
                continue;
            }
            if (parsed.front() == ">>") {   // if the string is for output redirection to an existing file, and append to it
                parsed.erase(parsed.begin());
                char* outputf = stocp(parsed.front());
                if ((outputfd = open(outputf, O_WRONLY | O_CREAT | O_APPEND, 0644)) == -1) {    // open the file to the output var
                    perror("open file");
                    exit(1);
                }
                parsed.erase(parsed.begin());
                continue;
            }
            if (parsed.front() == "$") {    // if the string is an environment variable
                parsed.erase(parsed.begin());
                char* env = stocp(parsed.front());
                char* env2 = getenv(env);
                argsca[i] = strdup(env2);
                delete env;
                parsed.erase(parsed.begin());
                i++;
                continue;
            }
            argsca[i] = stocp(parsed.front());  // convert the current string to char*
            parsed.erase(parsed.begin());   // remove it from the vector
            i++;
        }

        if ((parsed.size() > 0) && (parsed.front() == "|")) { // if it's a pipe command
            parsed.erase(parsed.begin()); 
            if (pipe(pipefd) == -1) {   // open a pipe to the pipefd var
                perror("pipe");
                exit(1);
            }
        }

        if ((pid = fork()) < 0) {   // fork
            perror("fork");
            exit(1);
        }

        if ((pid > 0) && (parsed.front() != "&")) { // make the global childpid var the pid of the child
            childpid = pid;
        }

        if (pid == 0) { // child proccess
            if (inputfd > 0) {  // if there is an input file open
                dup2(inputfd, 0);   // dup it to the stdin
                close(inputfd); // close the input file
            }

            if (outputfd > 0) { // if there is an output file open
                dup2(outputfd, 1);  // dup it to the stdout
                close(outputfd);    // close the output file
            }

            if (oldpipe > 0) {  // if there is a pipe open from the previous command
                dup2(oldpipe, 0);   // dup the read part of the pipe to the stdin
                close(oldpipe); // close the pipe
            }

            if (pipefd[WRITE] > 0) {    // if there is a new pipe open
                close(pipefd[READ]);    // close the read part
                dup2(pipefd[WRITE], 1); // dup the write part to the stdout
                close(pipefd[WRITE]);   // close the write part
            }

            execvp(argsca[0], argsca);  // call the exec
            perror("exec");
            exit(2);
            
        }
        else {  // parent proccess
            close(inputfd); // close the input file
            close(outputfd);    // close the output file
            close(pipefd[WRITE]); pipefd[WRITE] = -1;   // close the write part
            oldpipe = pipefd[READ]; // make the var oldpipe the same as the read part of the pipe

            for (int i=0; i<argsnum; i++) { // free the allocated memory
                delete argsca[i];
            }
            delete[] argsca;

            if (parsed.size() > 0) {    // if there's another arguement
                if (parsed.front() == "&") {    // if it's a command to run in the background
                    parsed.erase(parsed.begin());
                    parsed.erase(parsed.begin());
                    inputfd = -1;   // reset the file descriptors
                    outputfd = -1;
                    pipefd[READ] = -1; pipefd[WRITE] = -1;
                    oldpipe = -1;
                }
                else if (parsed.front() == ";") {   // if it's the end of the command
                    parsed.erase(parsed.begin());
                    inputfd = -1;   // reset the file descriptors
                    outputfd = -1;
                    pipefd[0] = -1; pipefd[1] = -1;
                    oldpipe = -1;
                    waitpid(pid, NULL, WUNTRACED);  // wait for the child to return
                }
            }
            else {
                children.push_back(pid);    // add the child pid to the wait vector
            }
        }

    }
    // after the command is over
    while(children.size() != 0){    // if there are pids in the vector
        waitpid(children.front(), NULL, WUNTRACED); // wait each of them
        children.erase(children.begin());
    }

    close(pipefd[READ]);
}

void check_for_alias_mod(vector<string>& parsed, unordered_map<string, string>& alias) {

    if (parsed[0] == "createalias") {   // the command is create alias
        add_alias(alias, parsed[1], parsed[2]); // add the alias to the struct
        parsed.erase(parsed.begin());   // remove the command from the vector
        parsed.erase(parsed.begin());
        parsed.erase(parsed.begin());
        if (parsed.size()>0 || parsed.front() == ";") {
            parsed.erase(parsed.begin());
        }
    }
    else if (parsed[0] == "destroyalias") { // if the command is destroy alias
        remove_alias(alias, parsed[1]); // remove alias from the struct
        parsed.erase(parsed.begin());   // remove the command from the string
        parsed.erase(parsed.begin());
        if (parsed.size()>0 || parsed.front() == ";") {
            parsed.erase(parsed.begin());
        }
    }
}