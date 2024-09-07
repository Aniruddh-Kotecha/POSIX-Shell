#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <fstream>
#include "functions.h"

using namespace std;

void displayPInfo(string pid, string wpid){
    string proc_status = "/proc/" + pid + "/status";
    string proc_exe = "/proc/" + pid + "/exe";


    ifstream status_file(proc_status);
    string state, memory;
    
    if (status_file.is_open()) {
        string line;
        
        while (getline(status_file, line)) {

            if (line.find("State:") != string::npos) {
                state = line.substr(line.find_last_of("SRZT"));
                state = state.substr(0, state.find_first_of(" \t"));
            } else if (line.find("VmSize:") != string::npos) {
                memory = line.substr(line.find_first_of("0123456789"));
                memory = memory.substr(0, memory.find_first_of(" \t"));
            }
        }
        if(pid == wpid)
            state = state + "+";

        status_file.close();
    }
    else{
        cout << "Error getting status for " << pid << endl;
        return;
    }

    char exe_path[1000];
    ssize_t len = readlink(&proc_exe[0], exe_path, sizeof(exe_path) - 1);

    string exe;

    if (len != -1) {
        exe_path[len] = '\0';
        exe = exe_path;
    } else {
        exe = "Unknown";
    }
    
    cout << "pid -- " << pid << endl;
    cout << "Process Status -- " << state << endl;
    cout << "memory -- " << memory << endl;
    cout << "Executable Path -- " << exe << endl;
}