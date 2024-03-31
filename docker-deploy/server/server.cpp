#include "server.hpp"


int Server::connect2Client() {
    // std::cout<<socket_fd<<std::endl;
    struct sockaddr_storage socket_addr;
    socklen_t socket_addr_len = sizeof(socket_addr);
    int client_connection_fd;
    // std::cout<<socket_fd<<std::endl;
    // printf("before accept\n");
    // std::cout<<socket_fd<<std::endl;
    client_connection_fd = accept(socket_fd, (struct sockaddr*)&socket_addr, &socket_addr_len);
    // std::cout<<client_connection_fd;
    if (client_connection_fd == -1) {
        std::cerr << "Error: cannot accept connection on socket" << std::endl;
        return -1;
    } //if

    return client_connection_fd;
}

int Server::buildServer() {
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    // printf("before");

    int status;
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        std::cerr << "Error: cannot get address info for host" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
    } //if
    // printf("getaddrinfo\n");

    socket_fd = socket(host_info_list->ai_family,
        host_info_list->ai_socktype,
        host_info_list->ai_protocol);
    // std::cout<<socket_fd<<std::endl;
    if (socket_fd == -1) {
        std::cerr << "Error: cannot create socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
    } //if
    // printf("socket\n");

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        std::cerr << "Error: cannot bind socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
    } //if

    status = listen(socket_fd, 512);
    if (status == -1) {
        std::cerr << "Error: cannot listen on socket" << std::endl;
        std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
        return EXIT_FAILURE;
    } //if
    // printf("listen\n");
    return EXIT_SUCCESS;
}

void Server::run() {
    int ready = buildServer();
    // cout<<"starting..."<<endl;
    if (ready == EXIT_FAILURE) {
        perror("Cannot connect\n");
        exit(EXIT_FAILURE);
    }

    connection* C;
    try {
            //Establish a connection to the database
            //Parameters: database name, user name, user password
          
            C = new connection("dbname=vkjsgika user=vkjsgika password=r4T0AK81uEhTYAnOGxyGjuKoz72zIdPB host=ruby.db.elephantsql.com port=5432");
            if (C->is_open()) {
                cout << "Opened database successfully: " << C->dbname() << endl;
            }
            else {
                cout << "Can't open database" << endl;
                return;
            }
        }
        catch (const std::exception& e) {
            cerr << e.what() << std::endl;
            return;
        }
        // createTable("sql/account.sql", C);
        // createTable("sql/stock.sql", C);
        // createTable("sql/position.sql", C);
        // createTable("sql/order.sql", C);

    // while (true) {
    //     int client_fd = connect2Client();
    //     // parse XML
    //     XMLHandler xmlhandler;
    //     std::string request = xmlhandler.receiveRequest(client_fd);
        
    //     string response = xmlhandler.handleXML(C, request);
    //     cout << response << endl;

    //     const char* response_xml = response.c_str();
    //     send(client_fd, response_xml, strlen(response_xml), 0);

    //     // freeaddrinfo(host_info_list);
    //     close(client_fd);
    // }

    // 创建一个线程池来处理客户端连接
    const int THREAD_POOL_SIZE = 5;
    std::vector<std::thread> threads;
    for (int i = 0; i < THREAD_POOL_SIZE; ++i) {
        threads.emplace_back([this, C]() {
            while (true) {
                int client_fd = connect2Client();
                XMLHandler xmlhandler;
                std::string request = xmlhandler.receiveRequest(client_fd);
                string response = xmlhandler.handleXML(C, request);
                cout << response << endl;
                const char* response_xml = response.c_str();
                send(client_fd, response_xml, strlen(response_xml), 0);
                close(client_fd);
            }
        });
    }

    // 等待所有线程完成
    for (auto& thread : threads) {
        thread.join();
    }

    C->disconnect();
}