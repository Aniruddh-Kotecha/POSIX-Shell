#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "functions.h"

using namespace std;

void executeRedirection(string prompt){

    vector<string> parts = splitPrompt(prompt, ' ');

    string command = parts[0];

    string inputFile;
    string outputFile;
    bool append = false;

    vector<char*> args;
    args.push_back(&command[0]);
    for (int i = 1; i < parts.size(); i++) {
        if (parts[i] == "<") {
            if (++i < parts.size()) 
                inputFile = parts[i];
            else 
                cerr << "Missing input file\n";
        } else if (parts[i] == ">") {
            if (++i < parts.size()) 
                outputFile = parts[i];
            else 
                cerr << "Missing output file\n";
            append = false;
        } else if (parts[i] == ">>") {
            if (++i < parts.size()) 
                outputFile = parts[i];
            else 
                cerr << "Missing output file\n";
            append = true;
        } else {
            args.push_back(&parts[i][0]); 
        }
    }
    args.push_back(nullptr); 

    pid_t pid = fork();

    if (pid == 0) { 
        if (!inputFile.empty()) {
            int fd = open(&inputFile[0], O_RDONLY);
            if (fd == -1) {
                perror("Error: Opening input file");
                exit(1);
            }
            dup2(fd, STDIN_FILENO); 
            close(fd);
        }
        
        if (!outputFile.empty()) {
            int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
            int fd = open(&outputFile[0], flags, 0644);
            if (fd == -1) {
                perror("Error: Opening output file");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO); 
            close(fd);
        }
        
        execvp(&command[0], args.data());
        perror("Error: execvp");
        exit(1);
    } else if (pid > 0) { 
        int status;
        waitpid(pid, &status, 0); 
    } else {
        perror("Error: fork");
    }

}