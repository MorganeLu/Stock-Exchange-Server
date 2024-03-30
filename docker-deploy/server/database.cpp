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

string getCurrTime() {
    std::time_t currentTime = std::time(nullptr);
    struct tm* localTime = localtime(&currentTime);

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
    }

    // Check account existance
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID =" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() != 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account already exists</error>\n";
    }

    sql = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" + to_string(account_id) + "," + to_string(balance) + ");";
    executeSQL(C, sql);
    return "<created id=\"" + to_string(account_id) + "\"/>\n";
}

string addPosition(connection* C, string symbol, int account_id, float amount) {
    if (amount < 0) {
        return "<error id=\"" + to_string(account_id) + "\">Negative amount is not allowed</error>\n";
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
    sql = "SELECT STOCK.STOCK_ID FROM STOCK WHERE SYMBOL=\'" + symbol + "\';";
    // result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        sql = "INSERT INTO STOCK (SYMBOL) VALUES (" + symbol + ");";
        executeSQL(C, sql);
    }

    sql = "SELECT COUNT(*) FROM STOCK WHERE SYMBOL =\'" + symbol + "\';";

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
    sql = "INSERT INTO POSITION (STOCK_ID, ACCOUNT_ID, AMOUNT) VALUES (" +
        to_string(stock_id) + "," + to_string(account_id) + "," + to_string(amount) +
        ")ON CONFLICT (STOCK_ID, ACCOUNT_ID) DO UPDATE SET AMOUNT = POSITION.AMOUNT + " + to_string(amount) + ";";
    executeSQL(C, sql);
    return "<created sym=\"" + to_string(symbol) + "\" id=\"" + to_string(account_id) + "\"/>\n";
}

string openOrder(connection* C, string symbol, int account_id, int trans_id, float amount, int price) {
    // check account
    string sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID =" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account not exists</error>\n";
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
    }
    int account = res.at(0).at(0).as<int>();
    int stock_id = res.at(0).at(1).as<int>();
    float amount = res.at(0).at(2).as<int>();
    int price = res.at(0).at(3).as<int>();
    int status = res.at(0).at(4).as<int>();
    if (account != account_id) {
        return "<error id=\"" + to_string(trans_id) + "\">Account does not have this order</error>";
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

    return "<canceled id=\"" + to_string(trans_id) + "\">xxx</canceled>";
}

string executeOrder(connection* C, string symbol, int account_id, float amount, int price) {
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account not exists</error>\n";
    }
    if (amount == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Ammount cannot be 0</error>\n";
    }

    int stock_id;
    if (amount > 0) { // buy
        // check balance and substract
        sql = "SELECT BALANCE FROM ACCOUNT WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
        getResult(C, sql, res);
        int balance = res.at(0).at(0).as<int>();
        if (balance < amount * price) {
            return "<error  sym=\"" + to_string(symbol) + "\" amount=\"" + to_string(amount) +
                "\" limit=\"" + to_string(price) + "\">Balance is not enough</error>\n";
        }
        sql = "UPDATE ACCOUNT SET BALANCE=BALANCE-" + to_string(amount * price) + " WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
        executeSQL(C, sql);

        // 需要加上这个吗？
        // Add or update position
        // sql = "INSERT INTO POSITION (STOCK_ID, ACCOUNT_ID, AMOUNT) VALUES (" + to_string(stock_id) + ", " + to_string(account_id) + ", " + to_string(amount) +
        //     ") ON CONFLICT (STOCK_ID, ACCOUNT_ID) DO UPDATE SET AMOUNT = POSITION.AMOUNT + " + to_string(amount) + ";";
    }
    else {  // sell
        // 是否需要将amount转为正数?
        amount = abs(amount);
        // check position and substract
        sql = "SELECT POSITION.AMOUNT, POSITION.STOCK_ID FROM POSITION, STOCK WHERE STOCK.SYMBOL=" + to_string(symbol) + " AND STOCK.STOCK_ID = POSITION.STOCK_ID AND "
            "POSITION.ACCOUNT_ID=" + to_string(account_id) + ";";
        getResult(C, sql, res);
        if (res.size() == 0) {
            return "<error id=\"" + to_string(account_id) + "\">Account does not have this stock</error>\n";
        }
        int original_amount = res.at(0).at(0).as<int>();
        int stock_id = res.at(0).at(1).as<int>();
        if (original_amount < amount) {
            return "<error id=\"" + to_string(account_id) + "\">Amount is not enough</error>\n";
        }
        else if (original_amount == amount) {
            // delete data from position
            sql = "DELETE FROM POSITION WHERE POSITION.STOCK_ID=" + to_string(stock_id) + " AND POSITION.ACCOUNT_ID=" + to_string(account_id) + ";";
            executeSQL(C, sql);
        }
        else if (original_amount > amount) {
            //update data in position
            sql = "UPDATE POSITION.AMOUNT SET POSITION.AMOUNT=POSITION.AMOUNT-" + to_string(amount) +
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

    // add sell order
    // get all buy orders
    // while(true){ // iterate all buy orders
    //     get all sell orders
    //         
    //}

    // 是不是应该放到 if sell 里?
    work W(*C);
    // find all matched buy orders
    string sqlMatch = "SELECT TRANS_ID, ACCOUNT_ID, AMOUNT, PRICE FROM \"ORDER\" "
        "WHERE SYMBOL = '" + symbol + "' AND "
        "STATUSS = 'OPEN' AND " +
        "(AMOUNT > 0  AND PRICE >= " + to_string(price) + ") "
        "ORDER BY ORDER_TIME ASC, PRICE ASC;";

    result matchingOrders = W.exec(sqlMatch);

    if (!matchingOrders.empty()) {
        for (auto order : matchingOrders) {
            int matchingAccountId = order["ACCOUNT_ID"].as<int>();
            float matchingAmount = order["AMOUNT"].as<float>();
            int matchingPrice = order["PRICE"].as<int>();

            int executionPrice = order["ORDER_TIME"].as<string>() <= getCurrTime() ? matchingPrice : price;
            float executionAmount = min(abs(amount), abs(matchingAmount));

            updateBalancesAndPositions(W, C, matchingAccountId, account_id, symbol, executionAmount, executionPrice);

            markOrdersAsExecuted(W, order["TRANS_ID"].as<int>(), account_id, executionAmount, executionPrice);

            amount -= executionAmount;

            if (amount == 0) {
                string deletesql = "DELETE FROM POSITION WHERE POSITION.STOCK_ID=" + to_string(stock_id) + " AND POSITION.ACCOUNT_ID=" + to_string(account_id) + ";";
                W.exec(deletesql);
                W.commit();
                break;
            }
        }
    }
    if (amount != 0) {
        sql = "INSERT INTO ORDER VALUES (" + to_string(account_id) + "," + to_string(stock_id) + "," + to_string(amount)
            + "," + to_string(price) + ", OPEN, " + getCurrTime() + ");";
        executeSQL(C, sql);
    }
    W.commit();

    //  应该返回什么?
    // return "<opened sym=\"" + to_string(symbol) + "\" amount=\"" + to_string(amount) + "\" limit=\"" +
    //     to_string(price) + "\" id=\"" + to_string(n) + "\"/>\n";
    return "";
}


void updateBalancesAndPositions(work& W, connection* C, int buyerId, int sellerId, string symbol, float amount, int price) {
    float totalCost = amount * price;

    // update buyer's balance (subtract total cost)
    string updateBuyerBalance = "UPDATE ACCOUNT SET BALANCE = BALANCE - " + to_string(totalCost) +
        " WHERE ACCOUNT_ID = " + to_string(buyerId) + ";";
    W.exec(updateBuyerBalance);

    // update seller's balance (add total cost)
    string updateSellerBalance = "UPDATE ACCOUNT SET BALANCE = BALANCE + " + to_string(totalCost) +
        " WHERE ACCOUNT_ID = " + to_string(sellerId) + ";";
    W.exec(updateSellerBalance);

    string sql = "SELECT COUNT(*) FROM STOCK WHERE SYMBOL =\'" + symbol + "\';";
    result res;
    getResult(C, sql, res);
    int stock_id = res.at(0).at(0).as<int>();

    // update or insert buyer's position
    string updateBuyerPosition = "INSERT INTO POSITION (STOCK_ID, ACCOUNT_ID, AMOUNT) VALUES (" +
        to_string(stock_id) + ", " + to_string(buyerId) + ", " + to_string(amount) +
        ") ON CONFLICT (STOCK_ID, ACCOUNT_ID) DO UPDATE SET " +
        "AMOUNT = POSITION.AMOUNT + EXCLUDED.AMOUNT;";
    W.exec(updateBuyerPosition);

    // update seller's position
    string updateSellerPosition = "UPDATE POSITION SET AMOUNT = AMOUNT - " + to_string(amount) +
        " WHERE ACCOUNT_ID = " + to_string(sellerId) +
        " AND STOCK_ID = (SELECT STOCK_ID FROM STOCK WHERE SYMBOL = '" + symbol + "');";
    W.exec(updateSellerPosition);
}

void markOrdersAsExecuted(work& W, int orderId, int accountId, float amount, int price) {
    string markOrderExecuted = "UPDATE \"ORDER\" SET STATUSS = 'EXECUTED', AMOUNT = AMOUNT - " + to_string(amount) +
        ", PRICE = " + to_string(price) +
        " WHERE TRANS_ID = " + to_string(orderId) +
        " AND ACCOUNT_ID = " + to_string(accountId) + ";";
    W.exec(markOrderExecuted);
}


string query(connection* C, int trans_id, int account_id) {
    string msg = "<status id=" + to_string(trans_id) + ">\n";
    // check account
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT.ACCOUNT_ID=" + to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(account_id) + "\">Account not exists</error>\n";
    }

    // 这一个和下面一个有什么区别？
    // check trans_id
    sql = "SELECT ORDER.STOCK_ID, ORDER.AMOUNT, ORDER.PRICE, ORDER_TIME FROM ORDER WHERE "
        "ORDER.TRANS_ID=" + to_string(trans_id) + ";";
    getResult(C, sql, res);
    if (res.size() == 0) {
        return "<error id=\"" + to_string(trans_id) + "\">Order not exists</error>";
        // TBD order not exist
    }

    // order可能split了，查询结果可能有多个
    // find open
    sql = "SELECT ORDER.AMOUNT FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=OPEN;";
    getResult(C, sql, res);
    for (result::const_iterator it = res.begin(); it != res.end(); ++it) {
        msg += "/r<open shares=" + to_string(it[0].as<int>()) + "/>\n";
    }
    // msg += "/r<open shares=" + to_string(res.at(0).at(0).as<int>()) + "/>\n";

    // find cancel
    sql = "SELECT ORDER.AMOUNT, ORDER_TIME FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=CANCELED;";
    getResult(C, sql, res);
    for (result::const_iterator it = res.begin(); it != res.end(); ++it) {
        msg += "/r<canceled shares=" + to_string(it[0].as<int>()) + " time=" + to_string(it[1].as<string>()) + "/>\n";
    }
    // msg += "/r<canceled shares=" + to_string(res.at(0).at(0).as<int>()) + " time=" + to_string(res.at(0).at(1).as<string>()) + "/>\n";

    // find execute
    sql = "SELECT ORDER.AMOUNT, ORDER.PRICE, ORDER_TIME FROM ORDER WHERE ORDER.TRANS_ID=" + to_string(trans_id) + " AND ORDER.STATUSS=EXECUTED;";
    getResult(C, sql, res);
    for (result::const_iterator it = res.begin(); it != res.end(); ++it) {
        msg += "/r<canceled shares=" + to_string(it[0].as<int>()) + " price=" + to_string(it[1].as<int>()) + " time=" + to_string(it[2].as<string>()) + "/>\n";
    }
    // msg += "/r<canceled shares=" + to_string(res.at(0).at(0).as<int>()) + " price=" + to_string(res.at(0).at(1).as<int>()) + " time=" + to_string(res.at(0).at(2).as<string>()) + "/>\n";

    msg += "</status>";
    return msg;
}

