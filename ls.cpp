#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <vector>
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <stdexcept>
#include <cstring>
#include "functions.h"

using namespace std;

char* getFileName(char* file_path){
    int len = strlen(file_path);
    int index = -1;
    for(int i = len - 1; i >= 0; i--){
        if(file_path[i] == '/'){
            index = i;
            break;
        }
    }

    if(index != -1){
        int filename_len = len - index - 1;

        char* file_name = new char[filename_len + 1];
    
        int j = 0;
        for(int i = index + 1; i < len; i++)
            file_name[j++] = file_path[i];

        file_name[j] = '\0';

        return file_name;
    }
        
    return file_path;
}

int calculateBlocks(DIR* dirp, string wd, int option){
    
    int totalBlocks = 0;
    struct dirent * dp;
    struct stat wd_status;

    while ((dp = readdir(dirp)) != NULL) {
        string filename = dp->d_name;

        //skip hidden files for -l
        if(option == 2 && filename[0] == '.')
            continue;

        string filePath = wd + "/" + filename;
        if(stat(&filePath[0], &wd_status) != 0){
        //handle error
            
        }

        totalBlocks += wd_status.st_blocks;
    }
    return totalBlocks/2;
}
string getPermissions(struct stat &fileStat) {

    string permissions = "";

    // File type
    if (S_ISDIR(fileStat.st_mode)) permissions += 'd';
    else if (S_ISLNK(fileStat.st_mode)) permissions += 'l';
    else permissions += '-';

    // Owner permissions
    permissions += (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
    permissions += (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    permissions += (fileStat.st_mode & S_IXUSR) ? 'x' : '-';

    // Group permissions
    permissions += (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    permissions += (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    permissions += (fileStat.st_mode & S_IXGRP) ? 'x' : '-';

    // Others permissions
    permissions += (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    permissions += (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    permissions += (fileStat.st_mode & S_IXOTH) ? 'x' : '-';

    //cout << permissions << endl;
    return permissions;
}

string getListFile(string filename, struct stat &wd_status){

    string listFile = "";

    //get permissions
    listFile += getPermissions(wd_status);

    //get number of links
    listFile += " " + to_string(wd_status.st_nlink);

    // Get owner name
    struct passwd* pw = getpwuid(wd_status.st_uid);
    string ownerName = (pw != nullptr) ? pw->pw_name : "unknown";

    listFile += " " + ownerName;

    // Get group name
    struct group* gr = getgrgid(wd_status.st_gid);
    string groupName = (gr != nullptr) ? gr->gr_name : "unknown";

    listFile += " " + groupName;

    //add file size
    listFile += " " + to_string(wd_status.st_size);

    /*
        %b: Abbreviated month name (e.g., Jan, Feb, Mar, etc.).
        %d: Day of the month as a zero-padded decimal number (e.g., 01, 02, ..., 31).
        %Y: Year with century (e.g., 2024).
        %H: Hour (24-hour clock) as a zero-padded decimal number (e.g., 00, 01, ..., 23).
        %M: Minute as a zero-padded decimal number (e.g., 00, 01, ..., 59).

    */

    //get modification time
    char timeBuf[80];
    struct tm* timeInfo = localtime(&wd_status.st_mtime);
    strftime(timeBuf, sizeof(timeBuf), "%b %d %H:%M", timeInfo);

    string curTime = timeBuf;

    listFile += " " + curTime;

    //add filename
    listFile += " " + filename;

    return listFile;

}

void filesWithoutOption(string wd, vector<string>& files){

    struct stat st;

    if(stat(&wd[0], &st) != 0){
    //handle error
        string message = "bash: ls: ";
        try{
            perror(&message[0]);
        }
        catch(exception e){
            cout << e.what() << endl;
            return;
        } 
        
    }

    else if(S_ISREG(st.st_mode)){
        string filename = getFileName(&wd[0]);
        files.push_back(filename);
    }

    else{
        DIR* dirp = opendir(&wd[0]);
        struct dirent * dp;
        while ((dp = readdir(dirp)) != NULL) {
            string filename = dp->d_name;
            if(filename[0] != '.')
                files.push_back(dp->d_name);
        }

        closedir(dirp);
    }
    
}

void filesWithOption(string wd, int option, vector<string>& files){

    cout << wd << endl;

    struct stat st;

    if(stat(&wd[0], &st) != 0){
        string message = "bash: ls: ";
        try{
            perror(&message[0]);
        }
        catch(exception e){
            cout << e.what() << endl;
            return;
        }   
    }

    else if(S_ISREG(st.st_mode)){
        string filename = getFileName(&wd[0]);
        
        if(option == 1)
            files.push_back(filename);
        
        else if(option > 1){
            string listFile = "";
            
            if(option == 2 && filename[0] == '.')
                return;

            listFile = getListFile(filename,  st);
            files.push_back(listFile);
        }
    }
        

    else{

        DIR* dirp = opendir(&wd[0]);
        DIR* dirp_blocks = opendir(&wd[0]);
        struct dirent * dp;
        struct stat wd_status;

        if(option == 1){
            while ((dp = readdir(dirp)) != NULL) {
                string filename = dp->d_name;
                files.push_back(dp->d_name);
            }
        }

        else if(option > 1){
            int totalBlocks = calculateBlocks(dirp_blocks, wd, option);
            files.push_back(to_string(totalBlocks));

            while ((dp = readdir(dirp)) != NULL) {
                string listFile = "";
                string filename = dp->d_name;

                //skip hidden files for -l
                if(option == 2 && filename[0] == '.')
                    continue;

                string filePath = wd + "/" + filename;
                if(stat(&filePath[0], &wd_status) != 0){
                //handle error
                    
                }

                listFile = getListFile(filename,  wd_status);

                files.push_back(listFile);
            }
            //cout << "total " + to_string(totalBlocks) << endl;
        }
        closedir(dirp);
    }
    
}

vector<string> listFiles(string cwd, int option){
    
    vector<string> files;

    if(option == 0)
        filesWithoutOption(cwd, files);
    else
        filesWithOption(cwd, option, files);

    return files;
}