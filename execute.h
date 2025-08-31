#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <sys/types.h>

extern pid_t childpid;

// execute the parsed input
void execute(std::vector<std::string>, std::unordered_map<std::string, std::string>&);