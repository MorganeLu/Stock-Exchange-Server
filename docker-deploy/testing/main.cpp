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
#include "client.hpp"

int main(int argc, char* argv[]) {
    Client myclient(argv[1], argv[2]);

    string filename = "./xml/transactions01.xml";
    std::ifstream file(filename);
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        file.close();

        std::string xmlContent = buffer.str();

        myclient.run(xmlContent);
    }


    return 0;
}