#ifndef DATABASEWRITER_H
#define DATABASEWRITER_H

#include "libpq-fe.h"
#include "GameData.h"

using namespace std;

//const char* CONNECTIONSTRING = "user=postgres password=superman dbname=poker hostaddr=127.0.0.1 port=5432";

class DatabaseWriter
{
private:
	void InitDB ();
	string ConvertInt (int number);
	PGconn *ConnectDB ();
	void CheckSuccess (PGresult* res, string action);
	int  GetPlayerId (string& playerName);
	int  InsertPlayer (string& playerName);
	int  InsertMove (MoveData md, int playerid, PlayerData pd);
	int  InsertMoveSequence (int* moveIDs, int length, int playerAmount);
	void  InsertGame (GameData gd, int* moveSequences);
	
public:
	void CloseConn(PGconn *conn);
	void StoreGame(GameData gd);
	DatabaseWriter ();
};


#endif