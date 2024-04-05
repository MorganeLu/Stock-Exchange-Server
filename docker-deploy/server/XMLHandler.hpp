#ifndef _XMLHANDLER_H
#define _XMLHANDLER_H
#include "pugixml/pugixml.hpp"
#include "database.hpp"
#include <pqxx/pqxx>
#include <string>
#include<vector>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

class XMLHandler {
public:
    XMLHandler() {}
    ~XMLHandler() {}

    std::string receiveRequest(int client_fd);
    std::string handleXML(connection* C, const std::string& xmlContent);

private:
    void parseCreate(connection* C, const pugi::xml_node& createNode, std::string& response);
    void parseTransactions(connection* C, const pugi::xml_node& transactionsNode, std::string& response);
    std::string createAccount(connection* C, const pugi::xml_node& accountNode);
    std::string createSymbol(connection* C, const pugi::xml_node& symbolNode);
    std::string execOrder(connection* C, const pugi::xml_node& orderNode);
    std::string cancelOrderHelper(connection* C, const pugi::xml_node& cancelNode);
    std::string queryOrder(connection* C, const pugi::xml_node& queryNode);
};
#endif