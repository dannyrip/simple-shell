#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <queue>

// string to char pointer, free after
char* stocp(std::string);

// tokenize the string
std::vector<std::string> tokenize(std::string);

// add alias
void add_alias(std::unordered_map<std::string, std::string>&, std::string, std::string);

// remove alias
void remove_alias(std::unordered_map<std::string, std::string>&, std::string);

// replase any alias in the input
void replace_alias(std::vector<std::string>&, std::unordered_map<std::string, std::string>&);

// moidfy the user's history
void history_mod(std::queue<std::string>&, std::string);

// print history
void print_history(std::queue<std::string>);

// get element from history in position i 
std::string get_history_element(std::queue<std::string>, int);

// count arguements in command
int count_args(std::vector<std::string>);