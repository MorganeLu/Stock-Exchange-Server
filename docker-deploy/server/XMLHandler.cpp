#include "XMLHandler.hpp"
#include <iostream>

std::string receiveRequest(int client_fd) {
    // vector<char> req(BUFF_SIZE, 0);
    // int len = recv(client_fd, req.data(), BUFF_SIZE, 0);
    // if (len <= 0) {
    //     std::cout << "Failed to receive request." << std::endl;
    //     return;
    // }
    std::string header;
    char buffer;
    while (recv(client_fd, &buffer, 1, 0) > 0 && buffer != '\n') {
        header.push_back(buffer);
    }

    int xmlLength = std::stoi(header);

    if (xmlLength <= 0) {
        std::cerr << "Invalid XML length received." << std::endl;
        return "";
    }
    std::cout << "Total length: " << xmlLength << std::endl;

    std::vector<char> xmlData(xmlLength + 1, 0);

    int totalBytesRead = 0;
    while (totalBytesRead < xmlLength) {
        int bytesRead = recv(client_fd, xmlData.data() + totalBytesRead, xmlLength - totalBytesRead, 0);
        if (bytesRead <= 0) {
            std::cerr << "Failed to receive complete XML data." << std::endl;
            return "";
        }
        totalBytesRead += bytesRead;
    }

    std::string xmlContent(xmlData.begin(), xmlData.end());
    std::cout << "XML content: " << xmlContent << std::endl;
    return xmlContent;
}

std::string XMLHandler::handleXML(connection* C, const std::string& xmlContent) {
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_string(xmlContent.c_str());
    std::string response = "";
    if (!result || xmlContent.empty()) {
        response += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Illegal "
            "XML Format</error>\n";
    }
    if (result) {
        pugi::xml_node rootNode = doc.first_child();
        std::cout << "Processing: " << std::string(rootNode.name()) << std::endl;
        if (std::string(rootNode.name()) == "create") {
            parseCreate(C, rootNode, response);
        }
        else if (std::string(rootNode.name()) == "transactions") {
            std::cout << "this is transactions\n";
            parseTransactions(C, rootNode, response);
        }
        else {
            response += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<error>Illegal "
                "XML Tag</error>\n";
        }
    }
    return response;
}

void XMLHandler::parseCreate(connection* C, const pugi::xml_node& createNode, std::string& response) {
    response += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<results>\n";

    for (pugi::xml_node child = createNode.first_child(); child; child = child.next_sibling()) {
        if (std::string(child.name()) == "account") {
            // std::cout << "Account ID: " << child.attribute("id").value()
            //     << ", Balance: " << child.attribute("balance").value() << std::endl;
            response += createAccount(C, child);
        }
        else if (std::string(child.name()) == "symbol") {
            // std::cout << "Symbol: " << child.attribute("sym").value() << std::endl;
            // for (pugi::xml_node account = child.child("account"); account; account = account.next_sibling("account")) {
            //     std::cout << "  Account ID: " << account.attribute("id").value()
            //         << ", Shares: " << account.text().as_string() << std::endl;
            // }
            response += createSymbol(C, child);
        }
        else {
            response += "<error>Invalid create tag</error>\n";
        }
    }

    response += "</results>";
}

void XMLHandler::parseTransactions(connection* C, const pugi::xml_node& transactionsNode, std::string& response) {
    response += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<results>\n";

    for (pugi::xml_node child = transactionsNode.first_child(); child; child = child.next_sibling()) {
        if (std::string(child.name()) == "order") {
            // std::cout << "Order:\n" << "Symbol: " << child.attribute("sym").value()
            //     << ", Amount: " << child.attribute("amount").value()
            //     << ", Limit: " << child.attribute("limit").value() << std::endl;
            response += execOrder(C, child);
        }
        else if (std::string(child.name()) == "cancel") {
            // std::cout << "Query:\n" << "Id: " << child.attribute("id").value() << std::endl;
            response += cancelOrderHelper(C, child);
        }
        else if (std::string(child.name()) == "query") {
            // std::cout << "Cancel:\n" << "Id: " << child.attribute("id").value() << std::endl;
            response += queryOrder(C, child);
        }
        else {
            int accountId = child.parent().attribute("id").as_int();
            response += "<error id=\"" + to_string(accountId) + "\">Invalid transaction tag</error>\n";
        }
    }
    response += "</results>";
}

std::string XMLHandler::createAccount(connection* C, const pugi::xml_node& accountNode) {
    int accountId = accountNode.attribute("id").as_int();
    float balance = accountNode.attribute("balance").as_float();
    return addAccount(C, accountId, balance);
}

std::string XMLHandler::createSymbol(connection* C, const pugi::xml_node& symbolNode) {
    std::string symbol = symbolNode.attribute("sym").value();
    std::string response;
    for (pugi::xml_node accountNode : symbolNode.children("account")) {
        int accountId = accountNode.attribute("id").as_int();
        float amount = accountNode.text().as_float();
        response += addPosition(C, symbol, accountId, amount);
    }
    return response;
}

std::string XMLHandler::execOrder(connection* C, const pugi::xml_node& orderNode) {
    std::string symbol = orderNode.attribute("sym").value();
    int accountId = orderNode.parent().attribute("id").as_int();
    float amount = orderNode.attribute("amount").as_float();
    int limit = orderNode.attribute("limit").as_int();
    return executeOrder(C, symbol, accountId, amount, limit);
}

std::string XMLHandler::cancelOrderHelper(connection* C, const pugi::xml_node& cancelNode) {
    int orderId = cancelNode.attribute("id").as_int();
    int accountId = cancelNode.parent().attribute("id").as_int();
    return cancelOrder(C, accountId, orderId);
}

std::string XMLHandler::queryOrder(connection* C, const pugi::xml_node& queryNode) {
    int orderId = queryNode.attribute("id").as_int();
    int accountId = queryNode.parent().attribute("id").as_int();
    return query(C, orderId, accountId);
}

