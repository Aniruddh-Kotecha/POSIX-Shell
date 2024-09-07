#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <sys/stat.h>
#include <dirent.h>
#include "functions.h"

using namespace std;

bool searchItem(string wd, string key){
    bool found = false;
    
    try{

        //cout << endl;
        
        //cout << wd << endl;

        DIR* dirp = opendir(&wd[0]);
        struct dirent * dp;
        struct stat st;

        while ((dp = readdir(dirp)) != NULL) {
            string filename = dp->d_name;

            if(filename[0] == '.')
                continue;

            //cout << filename << endl;

            if(filename == key){
                found = true;
                break;
            }

            string filePath = wd + "/" + filename;
            if(stat(&filePath[0], &st) != 0){
            //handle error
                
            }

            else if(S_ISDIR(st.st_mode)){
                found = searchItem(filePath, key);
                if(found == true)
                    break;
            }
        }
    }catch(exception e) {
        cout << e.what() << endl;
    }
    

    return found;
}