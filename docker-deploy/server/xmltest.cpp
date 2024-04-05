#include<iostream>
#include <fstream>
#include <sstream>
#include <iostream>
#include "XMLHandler.hpp"

int main() {
    std::ifstream file("xml/transactions01.xml");
    // std::ifstream file("xml/create01.xml");
    std::stringstream buffer;
    if (file) {
        buffer << file.rdbuf();
        file.close();

        std::string xmlContent = buffer.str();

        XMLHandler handler;
        // handler.handleXML(xmlContent);
    }
    else {
        std::cout << "Unable to open file." << std::endl;
    }

    return 0;
}