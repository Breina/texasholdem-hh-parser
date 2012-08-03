#include "stdafx.h"
#include <string>
#include <iostream>
#include "libpq-fe.h"
#include "DatabaseWriter.h"
#include "GameData.h"

using namespace std;

PGconn* conn;

void DatabaseWriter::CloseConn (PGconn *conn)
{
	PQfinish(conn);
	getchar();
	exit(1);
}

PGconn* DatabaseWriter::ConnectDB ()
{
  PGconn *conn = NULL;

  // Make a connection to the database
  conn = PQconnectdb("user=postgres password=superman dbname=poker hostaddr=127.0.0.1 port=5432");

  // Check to see that the backend connection was successfully made
	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Connection to database failed");
		DatabaseWriter::CloseConn(conn);
	}

	printf("Connection to database - OK\n");

    return conn;
}

void DatabaseWriter::InsertPlayer (PlayerData pd)
{
	std::string sSQL;
	sSQL.append("INSERT INTO players (name) VALUES ('");
	sSQL.append(pd.name);
	sSQL.append("')");
 
	// Execute with sql statement
	PGresult *res = PQexec(conn, sSQL.c_str());

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        printf("Insert employee record failed");
        PQclear(res);
        CloseConn(conn);
    }

	printf("Insert employee record - OK\n");

	// Clear result
	PQclear(res);
}

void DatabaseWriter::StoreGame(GameData gd)
{
	InsertPlayer(gd.GetPlayer(0));


	system("PAUSE");
}

DatabaseWriter::DatabaseWriter ()
{
	conn = ConnectDB();

}