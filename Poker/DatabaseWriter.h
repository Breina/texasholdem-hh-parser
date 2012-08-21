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
	long  GetPlayerId (string& playerName);
	long  InsertPlayer (string& playerName);
	long  InsertMove (MoveData md, long playerid, PlayerData pd);
	long  InsertMoveSequence (long* moveIDs, int length, int playerAmount);
	void  InsertGame (GameData gd, long* moveSequences);
	
public:
	void CloseConn(PGconn *conn);
	void StoreGame(GameData gd);
	DatabaseWriter ();
};


#endif