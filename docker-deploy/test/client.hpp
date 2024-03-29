#ifndef CLIENT_HPP
#define CLIENT_HPP

#include<cstdlib>
#include<cstdio>
#include <cstring>
#include <iostream>
#include<vector>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

class Client{
private:
    const char *hostname;
    const char *port;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    int socket_fd;

public:
    Client():hostname(NULL), port(NULL){}
    Client(const char * host, const char * port): hostname(host), port(port){}

    int buildClient();
    int connect2Server();
    void run(string request);

    ~Client() {
        free(host_info_list);
        // close(socket_fd);
    }

};

#endif