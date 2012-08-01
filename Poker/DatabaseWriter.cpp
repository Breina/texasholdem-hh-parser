#include "stdafx.h"
#include <string>
#include <string>
#include "libpq-fe.h"
#include "DatabaseWriter.h"

using namespace std;

const char* CONNECTIONSTRING = "user=postgres password=superman dbname=poker hostaddr=127.0.0.1 port=5432";

void DatabaseWriter::CloseConn (PGconn *conn)
{
    PQfinish(conn);
	getchar();
    exit(1);
}

PGconn DatabaseWriter::*ConnectDB ()
{
  PGconn *conn = NULL;

  // Make a connection to the database
  conn = PQconnectdb(CONNECTIONSTRING);

  // Check to see that the backend connection was successfully made
    if (PQstatus(conn) != CONNECTION_OK)
    {
        printf("Connection to database failed");
        CloseConn(conn);
    }

  printf("Connection to database - OK\n");

  return conn;
}