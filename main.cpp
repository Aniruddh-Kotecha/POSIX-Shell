#include <iostream>
#include <unistd.h>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <sys/wait.h>
#include <csignal>
#include <limits>
#include <termios.h>
#include <dirent.h>
#include <set>
#include "functions.h"

using namespace std;

#define BUFFER_SIZE 200

volatile sig_atomic_t resetLoop = 0;
string wd, cwd, pwd;
char hostname[BUFFER_SIZE];
char username[BUFFER_SIZE];
char delimiters[] = " \t";
string input;
string searchWord;
pid_t c_pid;
vector<string> commands;
vector<string> dirContents;
vector<string> options;

string getWord(string input){
    int pos = input.rfind(' ');
    
    if (pos == string::npos) {
        return input;
    }
    
    return input.substr(pos + 1);

}

string getPath(string wd, string cwd){
    if(cwd.find(wd) != string::npos) cwd = "~" + cwd.substr(wd.length());
    return cwd;
}

string getParentPath(char* cwd){
    int len = strlen(cwd);
    int index = -1;
    for(int i = len - 1; i >= 0; i--){
        if(cwd[i] == '/'){
            index = i;
            break;
        }
    }

    string dir = cwd;

    if(index != -1){
        string parentPath = dir.substr(0, index);
        return parentPath;
    }
        
    return cwd;
}

bool startsWith(string str, string prefix) {
    
    if (prefix.length() > str.length()) {
        return false;
    }
    
    return str.compare(0, prefix.length(), prefix) == 0;
}

void displayTerminal(string uname, string hname, string wd, string cwd){
    cwd = getPath(wd, cwd);
    cout << uname << "@" << hname << ":" << cwd << "> ";
    cout.flush();
}

void handleCSignal(int signum){
    
    if (c_pid > 0) {
        return;
    }

    cout << "^C\n";
    cout.flush();
    displayTerminal(username, hostname, wd, cwd);
    input.clear();
    return;
        
}

void handleZSignal(int sig) {
    
    if (c_pid > 0) {
        kill(c_pid, SIGTSTP); // Pause the child process
    }
    else{
        cout << "\n";
        cout.flush();
        displayTerminal(username, hostname, wd, cwd);
        input.clear();
    }
    return;
}

void executeCommand(string prompt){

    if(prompt.find('|') != string::npos){
        executePipes(prompt);
        return;
    }

    if(prompt.find('<') != string::npos || prompt.find('>') != string::npos){
        executeRedirection(prompt);
        return;
    }

    char * token = strtok(&prompt[0], delimiters);
    
    if(token == nullptr)
        return;
    
    string tokenStr = token;
    
    if(tokenStr == "pwd"){
        cout << getPath(wd, cwd) << endl;
    }

    else if(tokenStr == "cd"){
        token = strtok(nullptr, delimiters);
        if(token != nullptr){
            string temp = cwd;
            string changeDir = token; 
            if(startsWith(changeDir, "~")){
                changeDir = wd + changeDir.substr(1);
            }
            token = strtok(nullptr, delimiters);
            if(token != nullptr){
                cout << "bash: cd: too many arguments\n";
            }
            else{
                cwd = changeDirectory(wd, pwd, cwd, changeDir);
                pwd = temp;

                options.clear();

                dirContents = getDirectoryContents(cwd);

                //options.insert(options.end(), posixCommands.begin(), posixCommands.end());
                options.insert(options.end(), dirContents.begin(), dirContents.end());
            }
            
        }
        else
            cwd = changeDirectory(wd, pwd, cwd, "");
    }

    else if(tokenStr == "echo"){
        token = strtok(nullptr, delimiters);
        string message = "";
        while(token != nullptr){
            message = message + token;
            token = strtok(nullptr, delimiters);
            if (token != nullptr) {
                message = message + " ";
            }
        }
        displayMessage(message);
    }

    else if(tokenStr == "ls"){

        vector<string> paths;
        vector<string> files;
        
        int option = 0;
        token = strtok(nullptr, delimiters);
        while(token != nullptr){
            string arg = token;
            if(arg == "-a" && option < 1)
                option = 1;
            
            else if(arg == "-a" && option == 2)
                option = 3;

            else if(arg == "-l" && option < 2){
                if(option == 1)
                    option = 3;
                else
                    option = 2;
            }
                
            

            else if(arg == "-al" || arg == "-la" and option < 3)
                option = 3;
            
            else if(arg[0] == '-' && arg != "-l" && arg != "-a" && arg != "-al" && arg != "-la"){
                cout << "bash: ls: Invalid option\n";
                break;
            } 
            
            else if(arg[0] == '-'){
                //do nothing
            }
            else
                paths.push_back(arg);

            token = strtok(nullptr, delimiters);
        }


        if(paths.empty()){
            files = listFiles(cwd, option);   

            for(auto i: files){
                cout << i << endl;
            }
        }

        else if(paths.size() == 1){
            string path = paths[0];
            if(path == "~")
                path = wd;
            else if(path == ".")
                path = cwd;
            else if(path == "..")
                path = getParentPath(&cwd[0]);
                
            files = listFiles(path, option);

            for(auto i: files){
                cout << i << endl;
            }
        }
                
        else{
            for(int i = 0; i < paths.size(); i++){
                string path = paths[i];
                if(path == "~")
                    path = wd;
                else if(path == ".")
                    path = cwd;
                else if(path == "..")
                    path = getParentPath(&cwd[0]);
                else
                    path = cwd + "/" + path;

                string displayPath = paths[i] == "~" ? path: paths[i];
                cout << displayPath << ":" << endl;

                files = listFiles(path, option);

                for(auto j: files){
                    cout << j << endl;
                }
                cout << endl;  
            }
        }
    }

    else if(tokenStr == "pinfo"){
        string wpid;
        token = strtok(nullptr, delimiters);
        wpid = to_string(getpid());
        if(token == nullptr){
            displayPInfo(wpid, wpid);
        }

        else{
            string pid = token;
            token = strtok(nullptr, delimiters);
            if(token != nullptr){
                cout << "Error: command pinfo: Too many arguments\n";
                return;
            }
            displayPInfo(pid, wpid);
            
        }
        
    }

    else if(tokenStr == "search"){
        token = strtok(nullptr, delimiters);
        if(token == nullptr){
            cout << "Error: Please input file to search\n";
            return;
        }

        string key = token;
        int found = searchItem(cwd, key);
        if(found == 1)
            cout << "true" << endl;
        else
            cout << "false" << endl;
    }

    else if(tokenStr == "history"){
        token = strtok(nullptr, delimiters);
        if(token == nullptr)
            printHistory(commands, 10);
        else{
            string temp = token;
            int num = stoi(temp);
            if(num > 20 || num < 1){
                cout << "Error: command history: Incorrect argument [1 - 20] \n";
                return;
            }
            token = strtok(nullptr, delimiters);
            if(token != nullptr){
                cout << "Error: Too many arguments for command: history\n";
                return;
            }
            printHistory(commands, num);
        }
    }

    else if(prompt != "exit"){
        vector<char*> args;
        args.push_back(token);

        bool is_back = false;

        token = strtok(nullptr, delimiters);
        while(token != nullptr){
            string arg = token;
            if(arg == "&"){
                is_back = true;
            }
            else{
                args.push_back(strdup(token));
            }
            
            token = strtok(nullptr, delimiters);
        }
        args.push_back(nullptr);
        
        c_pid = fork(); 

        if (c_pid < 0) { 
            perror("fork"); 
            return; 
        }
        //child
        else if(c_pid == 0){
            if (execvp(args[0], args.data()) < 0){
                string message = args[0];
                message = "No such command found: " + message;
                cout << message << endl;
                    
            }
            exit(1);
        }
        //parent
        else{
            int status;
            if(!is_back){
                pid_t result = waitpid(c_pid, &status, WUNTRACED);

                if (result > 0) {
                    if (WIFSTOPPED(status)) {
                        //kill(c_pid, SIGCONT); // Resume the child process
                    } else if (WIFEXITED(status)) {
                        return;
                    }
                }
            }
            else{
                cout << c_pid << endl;
            }
        }
    }

}

int main(){

    signal(SIGINT, handleCSignal);
    signal(SIGTSTP, handleZSignal);

    struct termios orig_termios;
    string prompt;

    vector<string> matches;
    int matchIndex = 0;

    char wd_arr[BUFFER_SIZE];

    if(gethostname(hostname, BUFFER_SIZE) == -1){
        perror("Error getting hostname");
        return 1;
    }

    if(!(getlogin_r(username, BUFFER_SIZE) == 0)){
        perror("Error getting username");
        return 1;
    }

    if(getcwd(wd_arr, BUFFER_SIZE) == NULL){
        perror("Error getting current working directory");
        return 1;
    }

    wd = wd_arr;

    commands = getHistory(wd);

    //set<string> posixCommands = {"echo", "ls", "pwd", "cd", "history", "search", "pinfo", "cat", "grep" "mkdir", "exit"};
    dirContents = getDirectoryContents(wd);

    //options.insert(options.end(), posixCommands.begin(), posixCommands.end());
    options.insert(options.end(), dirContents.begin(), dirContents.end());

    cwd = wd;
    
    char* semiToken;
    char* iOToken;
    string tokenStr;

    enableRawMode(orig_termios);
    
    do{

        int historyIndex = commands.size() - 1;
        displayTerminal(username, hostname, wd, cwd);

        while (true) {
            char c = '\0';
            ssize_t nread = read(STDIN_FILENO, &c, 1);

            if (nread == -1) continue;

            if(c == 4){
                disableRawMode(orig_termios);
                cout << endl;
                exit(1);
            }

            else if (c == '\x1b') { // Escape character detected
                char seq[2];
                if (read(STDIN_FILENO, &seq[0], 1) == 0) continue;
                if (read(STDIN_FILENO, &seq[1], 1) == 0) continue;

                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A':
                            if(historyIndex == -1) break;
                            input = commands[historyIndex];
                            historyIndex--;
                            cout << "\33[2K\r";
                            displayTerminal(username, hostname, wd, cwd);
                            cout << input;
                            cout.flush();
                            break;
                        // case 'B':
                        //     if(historyIndex == commands.size() - 1) break;
                        //     input = commands[++historyIndex];
                        //     cout << "\33[2K\r";
                        //     displayTerminal(username, hostname, wd, cwd);
                        //     cout << input;
                        //     cout.flush();
                        //     break;
                    }
                }
            }
            
            else if (c == '\t') {
                options.clear();
                dirContents = getDirectoryContents(wd);
                options.insert(options.end(), dirContents.begin(), dirContents.end());
                if(searchWord == "")
                    continue;
                matches = findMatches(searchWord, options);
                matchIndex = 0;

                if(matches.size() == 1){
                    int pos = input.rfind(' ');
                    input = input.substr(0, pos+1) + matches[matchIndex];
                    cout << "\33[2K\r";
                    displayTerminal(username, hostname, wd, cwd);
                    cout << input;
                    cout.flush();
                }
                
                else if(matches.size() > 1){
                    string possibleMatches = "";
                    int count = 0;
                    for(int i = 0 ;i < matches.size(); i++){
                        possibleMatches += matches[i];
                        count++;
                        if(count == 4){
                            possibleMatches += "\n";
                            count = 0;
                        }
                        else
                            possibleMatches += "\t";
                    }
                    cout << "\n" << possibleMatches << "\n";
                    displayTerminal(username, hostname, wd, cwd);
                    cout << input;
                    cout.flush();
                }
            }

            else if (c == '\n') { // Enter key detected
                prompt = input;
                cout << endl;
                input.clear();
                break;
            } else if (c == 127) { // Backspace key
                if (!input.empty()) {
                    input.pop_back();
                    cout << "\b \b"; // Erase the last character
                }
            } else {
                input += c;
                cout << c;
                matches.clear();
            }
            searchWord = getWord(input);
            cout.flush();
        }

        if(prompt != "")
            commands = addHistory(commands,prompt,wd);
        
        int start = 0;
        int end = prompt.find(';');

        while (end != string::npos) {
            string newPrompt = prompt.substr(start, end - start);
            if (!newPrompt.empty()) {
                executeCommand(newPrompt);
            }
            start = end + 1;
            end = prompt.find(';', start);
        }

        string newPrompt = prompt.substr(start);
        if (!newPrompt.empty()) {
            executeCommand(newPrompt);
        }
        
    }    
    while(prompt != "exit");
    disableRawMode(orig_termios);

    return 0;
}