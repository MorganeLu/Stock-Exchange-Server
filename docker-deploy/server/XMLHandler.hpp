#ifndef _XMLHANDLER_H
#define _XMLHANDLER_H
#include "pugixml/pugixml.hpp"
#include "database.hpp"
#include <pqxx/pqxx>
#include <string>

class XMLHandler {
public:
    XMLHandler() {}
    ~XMLHandler() {}

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