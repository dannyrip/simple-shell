#include <iostream>
#include <cstdlib>
#include <cstring>
#include "parsing.h"
using namespace std;

// string to char pointer, free after
char* stocp(string s) {
    char* cp = new char[s.length()+1];
    strcpy(cp, s.c_str());
    return cp;
}

// tokenize the string
vector<string> tokenize(string input) {
    vector<string> parsed;

    int l = input.length(); //length of input string
    int j = 0;  // iterator for vector
    parsed.push_back("");   // add empty string to vector
    for (int i=0; i<l; i++) {
        if ((input.at(i) == ' ') || (input.at(i) == '<') || (input.at(i) == '>') || (input.at(i) == '|') || (input.at(i) == '"') || (input.at(i) == ';') || (input.at(i) == '&') || (input.at(i) == '$')){  //if the current character is a special character
            if (parsed.back() != "") {  // if the last string of the vector is not empty
                parsed.push_back("");   // add empty string
                j++;    // increment the vector iterator
            }
            if (input.at(i) == ' ') {   // if current character is white space continue
                continue;
            }
            if (input.at(i) == '"') {   // if current character is double quotation mark
                i++;    // move to the next character
                while (input.at(i) != '"') {    // while the char is not double quote
                    parsed[j].push_back(input.at(i));   // add current character to current string
                    i++;    // move to next character
                }
                i++;    // ignore the quotes and continue
                continue;
            }
            parsed[j].push_back(input.at(i));   // add the special character to the string
            if ((input.at(i) == '>') && (input.at(i+1) == '>')){    // if character is for output redirection check if it's for adding to an existing file
                parsed[j].push_back(input.at(i+1));
                i++;
            }
            parsed.push_back("");   // add empty string to vector
            j++;    // increment the vector iterator
            continue;
        }
        parsed[j].push_back(input.at(i));   // add current character to current string
    }

    while (parsed.back() == "") {
        parsed.pop_back();
    }

    return parsed;
}

// add alias
void add_alias(unordered_map<string, string>& alias, string key, string value) {
    alias[key] = value;
}

// remove alias
void remove_alias(unordered_map<string, string>& alias, string key) {
    alias.erase(key);
}

// replase any alias in the input
void replace_alias(vector<string>& parsed, unordered_map<string, string>& alias) {

    unordered_map<string, string>::iterator mit;
    vector<string> temp;

    //check if there is an alias in the input
    for (mit=alias.begin(); mit!=alias.end(); mit++) {
        for (int i=0; i<(int)parsed.size(); i++) {
            if (parsed[i] == mit->first) {  // if there is an alias
                temp = tokenize(mit->second);   //tokenize it 
                int j = i;
                for (int k=0; k<(int)temp.size(); k++) {    // and put it in the alias' place
                    parsed.insert(parsed.begin()+j, temp[k]);
                    j++;
                }
                parsed.erase(parsed.begin()+j); // remove the alias from the input
            }
        }
    } 
}

// moidfy the users' history
void history_mod(queue<string>& history, string input) {

    if (history.size() == 0) {  // if this is the first input
        history.push(input);    // save it in history
    }
    else {
        if (history.back() != input) {  // if the last input is not the same as the last command
            history.push(input);    // save it in history
        }
        if (history.size() == 21) { // if history size is over 20
            history.pop();  // remove the least recent command from history
        }
    }
}

// print history
void print_history(queue<string> history) {

    int i=1;
    while (!history.empty()) {  // while the queue is not empty
        cout << i << '\t' << history.front() << endl;   // print the number and the command in the first place
        history.pop();  // remove the command from the first place
        i++;
    }
}

// get element from history in position i 
string get_history_element(queue<string> history, int i) {
    i-=1;
    for (int j=0; j<i; j++) {
        history.pop();
    }
    return history.front();
}

// count arguements in command
int count_args(vector<string> parsed) {
    int i = 0;
    while (parsed.size() > 0){      // loop until command ends
        if ((parsed.front() == "|") || (parsed.front() == ";")) {   // if there is a pipe or a ; break out of the loop
            break;
        }
        if ((parsed.front() == "<") || (parsed.front() == ">") || (parsed.front() == ">>")) { // if there is redirection file skip it
            parsed.erase(parsed.begin());
            parsed.erase(parsed.begin());
            continue;
        }
        if (parsed.front() == "$") {
            parsed.erase(parsed.begin());
            continue;
        }
        i++;    // increment counter
        parsed.erase(parsed.begin());   // remove first arguement
    }
    return i;
}