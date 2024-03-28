#include <cstdlib>
#include <cstdio>
#include "database.hpp"

using namespace std;
using namespace pqxx;

int main (int argc, char *argv[]) 
{

  //Allocate & initialize a Postgres connection object
  connection *C;

  try{
    //Establish a connection to the database
    //Parameters: database name, user name, user password
    C = new connection("dbname=stock user=postgres password=passw0rd");
    if (C->is_open()) {
      cout << "Opened database successfully: " << C->dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return 1;
    }
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return 1;
  }

    createTable("sql/account.sql", C);
    createTable("sql/stock.sql", C);
    createTable("sql/position.sql", C);
    createTable("sql/open_order.sql", C);
    createTable("sql/cancel_order.sql", C);
    createTable("sql/execute_order.sql", C);


  //Close database connection
  C->disconnect();

  return 0;
}