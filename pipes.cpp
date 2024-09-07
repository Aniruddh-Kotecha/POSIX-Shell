#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "functions.h"

using namespace std;

vector<string> splitPrompt(string prompt, char delimiter){
    
    vector<string> result;

    int start = 0;
    int end = prompt.find(delimiter);

    while (end != string::npos) {
        string newPrompt = prompt.substr(start, end - start);
        if (!newPrompt.empty()) {
            result.push_back(newPrompt);
        }
        start = end + 1;
        end = prompt.find(delimiter, start);
    }

    string newPrompt = prompt.substr(start);
    if (!newPrompt.empty()) {
        result.push_back(newPrompt);
    }
    return result;
}

void executePipes(string prompt){

    vector<string> commands = splitPrompt(prompt, '|');

    int num_commands = commands.size();
    int pipefds[num_commands - 1][2];

    for (int i = 0; i < num_commands - 1; ++i) {
        if (pipe(pipefds[i]) == -1) {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < num_commands; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return;
        }

        if (pid == 0) {

            char * token = strtok(&commands[i][0], " ");

            vector<char*> args;
            args.push_back(token);

            token = strtok(nullptr, " ");
            while(token != nullptr){
                string arg = token;
                args.push_back(strdup(token));
                
                token = strtok(nullptr, " ");
            }
            args.push_back(nullptr);

            // If not the first command, get input from the previous pipe
            if (i > 0) {
                dup2(pipefds[i-1][0], STDIN_FILENO);
            }

            // If not the last command, write output to the next pipe
            if (i < num_commands - 1) {
                dup2(pipefds[i][1], STDOUT_FILENO);
            }

            // Close all pipe ends in the child
            for (int j = 0; j < num_commands - 1; ++j) {
                close(pipefds[j][0]);
                close(pipefds[j][1]);
            }

            // Execute the command
            execvp(args[0], args.data());

            // If execvp fails
            if (execvp(args[0], args.data()) < 0){
                string message = args[0];
                message = "No such in pipe command found: " + message;
                cout << message << endl;
                return;
                    
            }
            //exit(1);
        }
    }

    for (int i = 0; i < num_commands - 1; ++i) {
        close(pipefds[i][0]);
        close(pipefds[i][1]);
    }

    // Wait for all child processes to complete
    for (int i = 0; i < num_commands; ++i) {
        wait(nullptr);
    }

    // for(string i: commands){
    //     cout << i << endl;
    // }

}

    