#ifndef SERVER_HPP
#define SERVER_HPP

#include<cstdlib>
#include<cstdio>
#include <cstring>
#include <iostream>
#include<vector>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "XMLHandler.hpp"

using namespace std;


class Server {
private:
    const char *hostname;
    const char *port;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;

    // std::string ip;
    // std::vector<int> client_fds;
    int socket_fd;

public:
    Server():hostname(NULL), port("12345"){}
    Server(const char *port):hostname(NULL), port(port){}

    int buildServer();
    int connect2Client();
    void run();

    ~Server() {
        free(host_info_list);
        close(socket_fd);
    }
};


#endif