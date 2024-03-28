#include "database.hpp"

void executeSQL(connection* C, string sql){
    work W(*C);
    W.exec(sql);
    W.commit();
}

// result getResult(connection *C, string sql){
//     work W(*C);
//     result R(W.exec(sql));
//     W.commit();
  
//     return R;
// }
void getResult(connection *C, string sql, result &res){
    work W(*C);
    res = result(W.exec(sql));
    W.commit();
}

void createTable(string SQLfile, connection *C){
    string sql;
    string curr;
    ifstream f(SQLfile);

    if (f.is_open()){
        while(getline(f, curr)){
            sql.append(curr);
        }
        f.close();
    }
    else{
        std::cout << "Cannot open file"<<endl;
        return;
    }
    executeSQL(C, sql);
}

void deleteTable(connection *C, string tableName){
    string sql = "DROP TABLE IF EXISTS " + tableName + " CASCADE;";
    executeSQL(C, sql);

}

string  addAccount(connection *C, int account_id, float balance){
    if(balance < 0){  //TBD: CAN?
        return "<error></error>\n";
        // TBD: ERROR XML
    }

    // Check account existance
    string sql;
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID ="+ to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if(res.size() != 0){
        return "<error id=\"" + to_string(account_id) + "\">Account already exists</error>\n";
        // TBD: ERROR XML, account exists
    }

    sql = "INSERT INTO ACCOUNT (ACCOUNT_ID, BALANCE) VALUES (" + to_string(account_id)+ "," + to_string(balance) +");";
    executeSQL(C, sql);
    return "<created id=\""+to_string(account_id)+"\"/>\n";
}

string addPosition(connection *C, string symbol, int account_id, int amount){
    if(amount < 0){
        return "<error id=\"" + to_string(account_id) + "\">xxx</error>\n";
        // TBD
    }

    string sql;
    // check account
    sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID ="+ to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if(res.size() == 0){
        return "<error id=\"" + to_string(account_id) + "\">Account not exists</error>\n";
    }

    // check stock existance
    sql = "SELECT STOCK_ID FROM STOCK WHERE SYMBOL ="+ to_string(symbol) + ";";
    // result res;
    getResult(C, sql, res);
    if(res.size() == 0){
        sql = "INSERT INTO STOCK (SYMBOL) VALUES (" + symbol +");";
        executeSQL(C, sql);
    }
    sql = "SELECT COUNT(*) FROM STOCK WHERE SYMBOL ="+ to_string(symbol) + ";";
    
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
    sql = "INSERT INTO POSITION (STOCK_ID, ACCOUNT_ID, AMOUNT) VALUES (" + to_string(account_id) +
        to_string(stock_id) + "," + to_string(account_id) + "," + to_string(amount) +
        "ON CONFLICT (STOCK_ID, ACCOUNT_ID) DO UPDATE SET AMOUNT = POSITION.AMOUNT + " + to_string(amount) + ";";
    executeSQL(C, sql);
    return "<created sym=\"" + to_string(symbol) + "\" id=\"" + to_string(account_id) + "\"/>\n";
}

string openOrder(connection *C, string symbol, int account_id, int amount, int price, int type){
    // check account
    string sql = "SELECT ACCOUNT_ID FROM ACCOUNT WHERE ACCOUNT_ID ="+ to_string(account_id) + ";";
    result res;
    getResult(C, sql, res);
    if(res.size() != 0){
        return "<error id=\"" + to_string(account_id) + "\">Account already exists</error>\n";
        // TBD: ERROR XML, account exists
    }

    // check if have this stock and amount
    sql = "SELECT STOCK_ID FROM STOCK WHERE SYMBOL ="+ to_string(symbol) + ";";
    getResult(C, sql, res);
    if(res.size() == 0){
        return "<error id=\"" + to_string(account_id) + "\">Symbol not exists</error>\n";
    }
    sql = "SELECT COUNT(*) FROM STOCK WHERE SYMBOL ="+ to_string(symbol) + ";";
    getResult(C, sql, res);
    int stock_id = res.at(0).at(0).as<int>();
    

    // TBD: 如果想要买，但是账户里的钱暂时不够，可以买吗？
    return "";
}

string cancelOrder(connection *C, string symbol, int account_id, int amount, int price, int type){
    return "";
}
string executeOrder(connection *C, string symbol, int account_id, int amount, int price, int type){
    return "";
}
