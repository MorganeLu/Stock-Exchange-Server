#include<cstdlib>
#include<cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <dirent.h>

#include "client.hpp"

using namespace std;

std::vector<std::string> GetFilesInDirectory(const std::string& folderPath) {
    std::vector<std::string> files;
    DIR* directory = opendir(folderPath.c_str());
    if (directory != NULL) {
        struct dirent* entry;
        while ((entry = readdir(directory)) != NULL) {
            if (entry->d_type == DT_REG) { // Regular file
                // cout << entry->d_name<< endl;
                files.push_back(entry->d_name);
            }
        }
        closedir(directory);
    } else {
        std::cerr << "Error opening directory " << folderPath << std::endl;
    }
    return files;
}


int testAll(Client& myclient){
    std::string folder_path = "./xml"; 
    std::vector<std::string> files = GetFilesInDirectory(folder_path);
    for(string filename: files){
        string file_path = folder_path+"/"+filename;
        cout << file_path << endl;
        std::ifstream file(file_path);
        std::stringstream buffer;
        if (file) {
            buffer << file.rdbuf();
            file.close();
            std::string xmlContent = buffer.str();
            myclient.run(xmlContent);
            return 0;
        }
    }
    return -1;
}

int main(int argc, char* argv[]) {
    const char* host = "127.0.0.1";
    const char* port = "12345";
    Client myclient(host, port);

    // for test 1 file
    string filename = "./xml/test5.xml";
    std::ifstream file(filename);
    cout << filename << endl;
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        file.close();
        std::string xmlContent = buffer.str();
        cout << xmlContent.size() << endl;
        myclient.run(xmlContent);
    }


    // testAll(myclient);

    return 0;
}