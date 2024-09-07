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

void enableRawMode(struct termios &ori_tio) {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &ori_tio);
    new_tio = ori_tio;
    new_tio.c_lflag &= ~(ECHO | ECHOE | ICANON); 
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_tio);
}

void disableRawMode(struct termios &ori_tio) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &ori_tio);
}

vector<string> getDirectoryContents(string wd) {
    vector<string> entries;
    DIR *dir = opendir(&wd[0]);
    if (dir != nullptr) {
        struct dirent *dp;
        while ((dp = readdir(dir)) != nullptr) {
            entries.push_back(dp->d_name);
        }
        closedir(dir);
    }
    return entries;
}

vector<string> findMatches(string prefix, vector<string> options) {
    vector<string> matches;
    for (auto option : options) {
        if (option.find(prefix) == 0) {
            matches.push_back(option);
        }
    }
    return matches;
}