#ifndef DATABASEWRITER_H
#define DATABASEWRITER_H

#include "libpq-fe.h"
#include "GameData.h"

using namespace std;

//const char* CONNECTIONSTRING = "user=postgres password=superman dbname=poker hostaddr=127.0.0.1 port=5432";

class DatabaseWriter
{
private:
	PGconn *ConnectDB ();
	void InsertPlayer (PlayerData pd);

public:
	void CloseConn(PGconn *conn);
	void StoreGame(GameData gd);
	DatabaseWriter ();
};


#endif