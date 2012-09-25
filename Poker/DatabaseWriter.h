#ifndef DATABASEWRITER_H
#define DATABASEWRITER_H

#include <unordered_map>
#include "libpq-fe.h"
#include "GameData.h"

using namespace std;

//const char* CONNECTIONSTRING = "user=postgres password=superman dbname=poker hostaddr=127.0.0.1 port=5432";

struct playerDat
{
	long id;
	int count;
};


class DatabaseWriter
{
private:
	std::unordered_map<std::string, playerDat> *players; // Buffers the player id's to reduce db latency
	long playerID;

	string ConvertInt (int number);
	PGconn *ConnectDB ();
	void CheckSuccess (PGresult* res, string action);
	
public:
	void BuildDB ();
	void InitDB ();
	void PopulatePlayerList ();
	void CloseConn(PGconn *conn);
	void StoreGame(GameData gd);
	DatabaseWriter (bool cachePlayers);
};


#endif