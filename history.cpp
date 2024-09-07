#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>

using namespace std;

vector<string> getHistory(string wd){
    string filename = "history.txt";
    string filepath = wd + "/" + filename;
    ifstream file(filepath);
    vector<string> commands;
    string command;

    if (file.is_open()) {
        while (getline(file, command)) {
            commands.push_back(command);
        }
        file.close();
    } else {
        cerr << "Unable to open file " << filename << endl;
    }

    return commands;

}

void printHistory(vector<string> commands, int num){

    int len = commands.size();
    num = num > len? len: num;
    for(int i = len - num; i < len; i++)
        cout << commands[i] << endl;
}

void setHistory(vector<string> commands, string wd){

    string filename = "history.txt";
    string filepath = wd + "/" + filename;
    ofstream file(filepath);

    if (file.is_open()) {
        for(int i = 0; i < commands.size(); i++)
            file << commands[i] << endl;
        file.close();
    } else {
        cerr << "Unable to open file " << filename << endl;
    }

    return;

}

vector<string> addHistory(vector<string> commands, string command, string wd){
    
    if(commands.size() < 20){
        commands.push_back(command);
    }
    else{
        for(int i = 0; i < 19; i++)
        commands[i] = commands[i+1];

        commands[19] = command;
    }
    

    setHistory(commands, wd);

    return commands;

}