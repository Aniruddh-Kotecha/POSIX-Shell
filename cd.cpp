#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include "functions.h"

using namespace std;

using namespace std;
#define BUFFER_SIZE 200

string changeDirectory(string wd, string pwd, string cwd, string dir){ 

    if(dir == "-"){
        if(pwd == ""){
            cout << "bash: cd: OLDPWD not set\n";
            return cwd;
        }
        cout << pwd << endl;
        dir = pwd;
    }
        

    if(dir == "~" || dir == "")
        dir = wd;

    if(chdir(&dir[0]) < 0){
        string message = "bash: cd: " + dir;
        try{
            perror(&message[0]);
        }
        catch(exception e){
            cout << e.what() << endl;
            return cwd;
        } 
    }

    char nwd_arr[BUFFER_SIZE];
    string nwd;

    if(getcwd(nwd_arr, BUFFER_SIZE) == NULL){
        perror("Error getting new working directory");
        return NULL;
    }

    nwd = nwd_arr;
    return nwd;
}