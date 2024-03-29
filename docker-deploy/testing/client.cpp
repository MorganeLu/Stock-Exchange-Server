#include "client.hpp"

int Client::connect2Server(){
    std::cout << "Connecting to " << hostname << " on port " << port << "..." << std::endl;

    int status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot connect to socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        // return -1;
    } // connect to the server
    return socket_fd;
}
 int Client::buildClient(){
    memset(&host_info, 0, sizeof(host_info)); // init
    
    host_info.ai_family   = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;

    int status;
    // cout<<"start building client"<<endl;
    std::cout<<"myServiering....:"<<hostname<<std::endl;
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        // return EXIT_FAILURE;
    } // get address info for host

    socket_fd = socket(host_info_list->ai_family, 
                host_info_list->ai_socktype, 
                host_info_list->ai_protocol);
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        // return EXIT_FAILURE;
    } // get socket

    return EXIT_SUCCESS;
}

void Client::run(string request){
    buildClient();
    connect2Server();
    cout << request.size() << endl;
    cout << request << endl;

    send(socket_fd, request.c_str(), request.length(), 0);

    char response[512];
    recv(socket_fd, response, 512, 0);
    cout << "REPSONSE:\n"<< endl;
    cout << response << endl;

    close(socket_fd);
}