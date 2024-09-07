#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include <vector>
#include <set>
using namespace std;

//cd
string changeDirectory(string wd, string pwd, string cwd, string dir);

//echo
void displayMessage(string message);

//ls
vector<string> listFiles(string cwd, int option);

//pinfo
void displayPInfo(string pid, string wpid);

//search
bool searchItem(string cwd, string key);

//history
vector<string> getHistory(string wd);
void printHistory(vector<string> commands, int num);
void setHistory(vector<string> commands, string wd);
vector<string> addHistory(vector<string> commands, string command, string wd);

//autocomplete
void enableRawMode(struct termios &ori_tio);
void disableRawMode(struct termios &ori_tio);
vector<string> getDirectoryContents(string wd);
vector<string> findMatches(string prefix, vector<string> options);

//pipes
void executePipes(string prompt);

//redirection
void executeRedirection(string command);

//util
vector<string> splitPrompt(string prompt, char delimiter);

#endif