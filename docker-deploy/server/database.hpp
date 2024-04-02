#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdlib>
#include <string>
#include <pqxx/pqxx>
#include <fstream>
#include <iostream>
#include <ctime>

using namespace std;
using namespace pqxx;

void executeSQL(connection* C, string sql);
result getResult(work& W, string sql);

time_t getCurrTime();

void createTable(string SQLfile, connection* C);
void deleteTable(connection* C, string tableName);

string addAccount(connection* C, int account_id, float balance);
string addPosition(connection* C, string symbol, int account_id, float amount);

string openOrder(connection* C, string symbol, int account_id, int trans_id, float amount, int price);
string cancelOrder(connection* C, int account_id, int trans_id);
string executeOrder(connection* C, string symbol, int account_id, float amount, int price);
void updateBalancesAndPositions(connection* C, int buyerId, int sellerId, string symbol, float amount, int price);
void markOrdersAsExecuted(connection* C, int orderId, int accountId, float amount, int price);
void matchSellOrders(connection* C, int buyerId, int stock_id, string symbol, float amount, int price);
void matchBuyOrders(connection* C, int sellerId, int stock_id, string symbol, float amount, int price);

string query(connection* C, int trans_id, int account_id);

#endif