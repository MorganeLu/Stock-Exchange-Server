#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "server.hpp"

int main(){
    const char * port = "12345";
    Server myserver(port);
    myserver.run();
    return 0;
}