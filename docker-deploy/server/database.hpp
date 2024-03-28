#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdlib>
#include <string>
#include <pqxx/pqxx>
#include <fstream>
#include <iostream>

using namespace std;
using namespace pqxx;

void executeSQL(connection* C, string sql);
result getResult(work &W, string sql);

void createTable(string SQLfile, connection *C);
void deleteTable(connection *C, string tableName);

string addAccount(connection *C, float balance);
string addPosition(connection *C, string symbol, int account_id, int amount);

string openOrder(connection *C, string symbol, int account_id, int amount, int price, int type);
string cancelOrder(connection *C, string symbol, int account_id, int amount, int price, int type);
string executeOrder(connection *C, string symbol, int account_id, int amount, int price, int type);

#endif