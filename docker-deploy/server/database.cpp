#include "database.hpp"

void executeSQL(connection* C, string sql) {
    work W(*C);
    W.exec(sql);
    W.commit();
}

void getResult(connection* C, string sql, result& res) {
    work W(*C);
    res = result(W.exec(sql));
    W.commit();
}

string getCurrTime(){
    std::time_t currentTime = std::time(nullptr);
    struct tm *localTime = localtime(&currentTime);

    std::time_t utcTime = std::mktime(localTime);
    return std::asctime(std::gmtime(&utcTime));
}

void createTable(string SQLfile, connection* C) {
    string sql;
    string curr;
    ifstream f(SQLfile);

    if (f.is_open()) {
        while (getline(f, curr)) {
            sql.append(curr);
        }
        f.close();
    }
    else {
        std::cout << "Cannot open file" << endl;
        return;
    }
    executeSQL(C, sql);
}

void deleteTable(connection* C, string tableName) {
    string sql = "DROP TABLE IF EXISTS " + tableName + " CASCADE;";
    executeSQL(C, sql);

}

string  addAccount(connection* C, int account_id, float balance) {
    if (balance < 0) {  //TBD: CAN?
        return "<error id=\"" + to_string(account_id) + "\">Negative blance is not allowed</error>\n";
        // TBD: ERROR XML
    }

    // Check account existance
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID =" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() != 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account already exists</error>\n";
        // TBD: ERROR XML, account exists
    }

    sql = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" + to_string(account_id) + "," + to_string(balance) + ");";
    executeSQL(C, sql);
    return "<created id=\"" + to_string(account_id) + "\"/>\n";
}

string addPosition(connection* C, string symbol, int account_id, float amount) {
    if (amount < 0) {
        return "<error id=\"" + to_string(account_id) + "\">Negative amount is not allowed</error>\n";
        // TBD
    }

    string sql;
    // check account
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID =" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account not exists</error>\n";
    }

    // check stock existance
    sql = "SELECT STOCK_ID FROM STOCK WHERE SYMBOL =" + to_string(symbol) + ";";
    // result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        sql = "INSERT INTO STOCK (SYMBOL) VALUES (" + symbol + ");";
        executeSQL(C, sql);
    }
    //这个是为了得到stock_id吗？可否用 SELECT STOCK_ID FROM STOCK WHERE SYMBOL = symbol
    sql = "SELECT COUNT(*) FROM STOCK WHERE SYMBOL =" + to_string(symbol) + ";";

    getResult(C, sql, res);
    int stock_id = res.at(0).at(0).as<int>();


    // update position
    // sql = "SELECT COUNT(*) FROM POSITION, ACCOUNT WHERE SYMBOL ="+ to_string(symbol) + ";";
    // if(getResult(C, sql) != 0){ // find
    //     sql = "SELECT POSITION.AMOUNT FROM POSITION "
    //         "WHERE ACCOUNT_ID=" + to_string(account_id) +" AND STOCK_ID=" + to_string(stock_id) + ";";
    //     int amount = getResult(C, sql);
    //     sql = "UPDATE POSITION SET AMOUNT = 2500.00 WHERE ACCOUNT_ID = 2;"
    //     executeSQL(C, sql);
    // }else{ // not find
    //     sql = "INSERT"
    // }

    //为什么两个account_id?
    sql = "INSERT INTO POSITION (STOCK_ID, ACCOUNT_ID, AMOUNT) VALUES (" + to_string(account_id) +
        to_string(stock_id) + "," + to_string(account_id) + "," + to_string(amount) +
        "ON CONFLICT (STOCK_ID, ACCOUNT_ID) DO UPDATE SET AMOUNT = POSITION.AMOUNT + " + to_string(amount) + ";";
    executeSQL(C, sql);
    return "<created sym=\"" + to_string(symbol) + "\" id=\"" + to_string(account_id) + "\"/>\n";
}

string openOrder(connection* C, string symbol, int account_id, int trans_id, float amount, int price) {
    // check account
    string sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID =" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    // 这里应该对不存在的账户报错？
    if (res.size() != 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account already exists</error>\n";
        // TBD: ERROR XML, account exists
    }

    // check if have this stock and amount
    sql = "SELECT STOCK_ID FROM STOCK WHERE SYMBOL =" + to_string(symbol) + ";";
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Symbol not exists</error>\n";
    }
    sql = "SELECT COUNT(*) FROM STOCK WHERE SYMBOL =" + to_string(symbol) + ";";
    getResult(C, sql, res);
    int stock_id = res.at(0).at(0).as<int>();

    // not enough balance

    return "";
}

string cancelOrder(connection* C, int account_id, int trans_id) {
    // find trans order
    string sql;
    sql = "SELECT ACCOUNT_ID, STOCK_ID, AMOUNT, PRICE, STATUSS"
        "FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=OPEN;";
    result res;
    getResult(C, sql, res);
    if (res.size() != 1) {
        return "<error id=\"" + to_string(trans_id) + "\">Order does not exist</error>";
        // TBD
    }
    int account = res.at(0).at(0).as<int>();
    int stock_id = res.at(0).at(1).as<int>();
    float amount = res.at(0).at(2).as<int>();
    int price = res.at(0).at(3).as<int>();
    int status = res.at(0).at(4).as<int>();
    if (account != account_id) {
        return "<error id=\"" + to_string(trans_id) + "\">Account does not have this order</error>"; // this order not belonf to this account
    }

    if (amount > 0) {     // buy
        // add balance
        sql = "UPDATE ACCOUNT SET BALANCE=BALANCR+" + to_string(price * amount) + " WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
        executeSQL(C, sql);

    }
    else {      // sell
        // add position
        sql = "INSERT INTO POSITION VALUES (" + to_string(stock_id) + to_string(account_id) + to_string(amount) + ");";
        executeSQL(C, sql);
    }
    // order status cancel
    sql = "UPDATE ORDER SET STATUSS=CANCELED WHERAE ORDER.TRANS_ID=" + to_string(trans_id) + ";";
    executeSQL(C, sql);

    return "<canceled id=\"" + to_string(trans_id) + "\">xxx</canceled>"; //TBD
}

string executeOrder(connection* C, string symbol, int account_id, float amount, int price) {
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account not exists</error>\n";
        // TBD account not exist
    }
    if (amount == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Ammount cannot be 0</error>\n";
        // TBD
    }

    int stock_id;
    if(amount > 0){ // buy
        // check balance and substract
        sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
        getResult(C, sql, res);
        int balance = res.at(0).at(0).as<int>();
        if (balance < amount * price) {
            return "<error  sym=\"" + to_string(symbol) + "\" amount=\"" + to_string(amount) +
                "\" limit=\"" + to_string(price) + "\">Balance is not enough</error>\n";
            // TBD
        }
        sql = "UPDATE ACCOUNT SET BALANCE=BALANCE-" + to_string(amount * price) + " WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
        executeSQL(C, sql);
    }
    else {  // sell
        // check position and substract
        sql = "SELECT POSITION.AMOUNT, POSITION.STOCK_ID FROM POSITION, STOCK WHERE STOCK.SYMBOL=" + to_string(symbol) + " AND STOCK.STOCK_ID = POSITION.STOCK_ID AND "
            "POSITION.ACCOUNT_ID=" + to_string(account_id) + ";";
        getResult(C, sql, res);
        if (res.size() == 0) {
            return "<error id=\"" + to_string(account_id) + "\">Account does not have this stock</error>\n";
            // TBD: account dont have this stock
        }
        int original_amount = res.at(0).at(0).as<int>();
        int stock_id = res.at(0).at(1).as<int>();
        if(original_amount < amount){
            return "<error></error>\n";
            //TBD
        }
        else if (original_amount == amount) {
            // delete data from position
            sql = "DELETE FROM POSITION WHERE POSITION.STOCK_ID=" + to_string(stock_id) + " AND POSITION.ACCOUNT_ID=" + to_string(account_id) + ";";
            executeSQL(C, sql);
        }
        else if (original_amount > amount) {
            //update data in position
            sql = "UPDATE POSITION.AMOUNT SET POSITION.BALANCE=POSITION.BALANCE-" + to_string(amount) +
                " WHERE POSITION.STOCK_ID=" + to_string(stock_id) + " AND POSITION.ACCOUNT_ID=" + to_string(account_id) + ";";
            executeSQL(C, sql);
        }
    }
    
    // add new order
    sql = "INSERT INTO ORDER VALUES (" + to_string(account_id) + "," + to_string(stock_id) + "," + to_string(amount) 
        + "," + to_string(price) + ", OPEN, " + getCurrTime() + ");";
    executeSQL(C, sql);

    // check whether overlap and need execute
    // TBD



    return "";
}

string query(connection *C, int trans_id, int account_id){
    string msg = "<status id= " + to_string(trans_id) + ">\n";
    // check account
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if(res.size() == 0){
        return "<error>xxx</error>\n"; 
        // TBD account not exist
    }

    // check trans_id
    sql = "SELECT ACCOUNT_ID, STOCK_ID, AMOUNT, PRICE, STATUSS"
        "FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + ";";
    getResult(C, sql, res);
    if(res.size()!=1){
        return "<error></error>";
        // TBD order not exist
    }

    // select
    sql = "SELECT ORDER.STOCK_ID, ORDER.AMOUNT, ORDER.PRICE, ORDER_TIME FROM ORDER WHERE "
        "ORDER.TRANS_ID=" + to_string(trans_id) + ";";
    getResult(C, sql, res);
    if(res.size()==0){
        return "<error></error>";
        // TBD order not exist
    }

    // find open
    sql = "SELECT ORDER.AMOUNT FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=OPEN;";
    getResult(C, sql, res);
    msg += "/r<open shares=" + to_string(res.at(0).at(0).as<int>()) + "/>\n";

    // find cancel
    sql = "SELECT ORDER.AMOUNT, ORDER_TIME FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=CANCELED;";
    getResult(C, sql, res);
    msg += "/r<canceled shares=" + to_string(res.at(0).at(0).as<int>()) + " time=" + to_string(res.at(0).at(1).as<string>()) + "/>\n";

    // find execute
    sql = "SELECT ORDER.AMOUNT, ORDER.PRICE, ORDER_TIME FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=EXECUTED;";
    getResult(C, sql, res);
    msg += "/r<canceled shares=" + to_string(res.at(0).at(0).as<int>()) + " price=" + to_string(res.at(0).at(1).as<int>())+ " time=" + to_string(res.at(0).at(2).as<string>()) + "/>\n";
    
    msg += "</status>";
    return msg;
}

