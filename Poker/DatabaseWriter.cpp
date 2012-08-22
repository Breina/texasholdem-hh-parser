#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include "libpq-fe.h"
#include "DatabaseWriter.h"
#include "GameData.h"

using namespace std;

PGconn* conn;
//extern const int MAXPLAYERS;
//extern const int MAXMOVES;

//extern const int PREFLOP =	0;
//extern const int FLOP =	1;
//extern const int TURN =	2;
//extern const int RIVER =	3;
const int NONE = 0;

const int DBMAXPLAYERS = 10;
const int DBMAXROUNDS = 6;

void DatabaseWriter::InitDB ()
{
	PQexec(conn, "TRUNCATE moves CASCADE");
	PQexec(conn, "TRUNCATE players CASCADE;");
	PQexec(conn, "INSERT INTO moves VALUES (0, 0, 0, 0, 0, 0, 0, 0, 0, 0);");
	PQexec(conn, "INSERT INTO movesequences (movesequenceid) VALUES (0);");
}

string DatabaseWriter::ConvertInt(int number)
{
   stringstream ss;//create a stringstream
   ss << number;//add number to the stream
   return ss.str();//return a string with the contents of the stream
}

void DatabaseWriter::CloseConn (PGconn *conn)
{
	PQfinish(conn);
	//getchar();
	system("PAUSE");
	exit(1);
}

PGconn* DatabaseWriter::ConnectDB ()
{
	PGconn *conn = NULL;
	conn = PQconnectdb("user=postgres password=superman dbname=poker hostaddr=127.0.0.1 port=5432");

	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Connection to database failed");
		DatabaseWriter::CloseConn(conn);
		exit(1);
	}

	printf("Connection to database - OK\n");

    return conn;
}

void DatabaseWriter::CheckSuccess (PGresult* res, string action)
{
	if (PQresultStatus(res) == PGRES_COMMAND_OK ||
		PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		//printf("%s - OK\n", action.c_str());
		return;
	}
	printf("%s failed. Reason: ", action.c_str());

	switch (PQresultStatus(res))
	{
	case PGRES_EMPTY_QUERY:
		cout << "Empty query";
		break;
	case PGRES_BAD_RESPONSE:
		cout << "Bad response";
		break;
	case PGRES_NONFATAL_ERROR:
		cout << "Nonfatal error";
		break;
	case PGRES_FATAL_ERROR:
		cout << "Fatal error";
	}
	cout << endl << PQresultErrorMessage(res) << endl;

	PQclear(res);
	CloseConn(conn);	
}

long DatabaseWriter::GetPlayerId (string& playerName)
{
	std::string sSQL;
	sSQL.append("UPDATE players SET count = count + 1 WHERE name='");
	sSQL.append(playerName);
	sSQL.append("' RETURNING playerid");

	PGresult *res = PQexec(conn, sSQL.c_str());
	CheckSuccess(res, "Fetch playerid");
	long ret;
	if (PQntuples(res) != 0)
	{
		ret = atoi(PQgetvalue(res, 0, 0));
	}
	else
	{
		ret = InsertPlayer(playerName);
	}

	PQclear(res); // THIS FUCKING PIECE OF SHIT CODE WAS MISSING, CAUSING A MEMORY LEAK HOLDING THIS PROJECT BACK FOR WEEKS. FUKA-U CODE! FUKA YU WELL!!

	return ret;
}

long DatabaseWriter::InsertPlayer (string& playerName)
{
	std::string sSQL;
	sSQL.append("INSERT INTO players (name, count) VALUES ('");
	sSQL.append(playerName);
	sSQL.append("', '1') RETURNING playerid");
 
	PGresult *res = PQexec(conn, sSQL.c_str());
	CheckSuccess(res, "Insert player record");

	long ret = atoi(PQgetvalue(res, 0, 0));

	PQclear(res);

	return ret;
}

long DatabaseWriter::InsertMove (MoveData md, long playerid, PlayerData pd)
{
	int action;
	switch (md.move)
	{
	case SKIP:
		return NONE;
	case FOLD:
		action = 1;
		break;
	case CALL:
		action = 2;
		break;
	case CHECK:
		action = 3;
		break;
	case RAISE:
		action = 4;
		break;
	default:
		return NONE;	// Will handle SKIP
	}

	int costToCall = md.toCall - md.currentBet;
	//float potOdds = (costToCall == 0) ? 0 : md.pot / costToCall;

	std::string sSQL;
	sSQL.append("INSERT INTO moves VALUES (");
	sSQL.append("DEFAULT");				// moveid
	sSQL.append(", '");
	sSQL.append(ConvertInt(action));	// action
	sSQL.append("', '");
	sSQL.append(ConvertInt(md.pot));	// pot
	sSQL.append("', '");
	sSQL.append(ConvertInt(md.toCall));	// toCall
	sSQL.append("', '");
	sSQL.append(ConvertInt(md.stack));	// stack
	sSQL.append("', '");
	sSQL.append(ConvertInt(md.bet));	// bet
	sSQL.append("', '");
	sSQL.append(ConvertInt(playerid));	// playerid
	sSQL.append("', '");
	sSQL.append(pd.card1);				// card1
	sSQL.append("', '");
	sSQL.append(pd.card2);				// card2
	sSQL.append("', '");
	sSQL.append(ConvertInt(md.currentBet));	// currentBet
	sSQL.append("') RETURNING moveid");

	PGresult *res = PQexec(conn, sSQL.c_str());
	CheckSuccess(res, "Insert move record");

	long ret = atoi(PQgetvalue(res, 0, 0));

	PQclear(res);

	return ret;
}

long DatabaseWriter::InsertMoveSequence (long* moveIDs, int length, int playerAmount)
{
	int lap = 0;
	int playerPos = 0;
	int dblap = 0;
	int dbpos = 0;

	long move = 0;
	bool finished = false;

	std::string sSQL;
	sSQL.append("INSERT INTO movesequences VALUES (DEFAULT, '");
	for (;;)
	{
		if (dbpos == playerPos && dblap == lap)
		{
			sSQL.append(ConvertInt(moveIDs[move]));

			playerPos++;
			if (playerPos == playerAmount)
			{
				playerPos = 0;
				lap++;
			}
		}
		else
		{
			sSQL.append("0");
		}

		dbpos++;
		if (dbpos == DBMAXPLAYERS)
		{
			dbpos = 0;
			dblap++;
		}

		move++;
		if (move == length)
			break;

		sSQL.append("', '");
	}
	sSQL.append("') RETURNING movesequenceid");

	PGresult *res = PQexec(conn, sSQL.c_str());
	CheckSuccess(res, "Insert moveSequence record");

	long ret = atoi(PQgetvalue(res, 0, 0));

	PQclear(res);

	return ret;
}

void DatabaseWriter::InsertGame (GameData gd, long* moveSequences)
{
	std::string sSQL;
	sSQL.append("INSERT INTO games (gameid, sb, bb, limittype, gametype, playeramount, preflop, flop, turn, river) VALUES ('");
	sSQL.append(gd.GetGameID());			// gameID
	sSQL.append("', '");
	sSQL.append(ConvertInt(gd.GetSB()));	// sb
	sSQL.append("', '");
	sSQL.append(ConvertInt(gd.GetBB()));	// bb
	sSQL.append("', '");
	sSQL.append(gd.GetLimitType());			// limitType
	sSQL.append("', '");
	sSQL.append(gd.GetGameType());			// gameType
	sSQL.append("', '");
	sSQL.append(ConvertInt(gd.GetPlayerAmount()));		// pAmount
	sSQL.append("', '");
	sSQL.append(ConvertInt(moveSequences[PREFLOP]));
	sSQL.append("', '");
	sSQL.append(ConvertInt(moveSequences[FLOP]));
	sSQL.append("', '");
	sSQL.append(ConvertInt(moveSequences[TURN]));
	sSQL.append("', '");
	sSQL.append(ConvertInt(moveSequences[RIVER]));
	sSQL.append("')");

	PGresult *res = PQexec(conn, sSQL.c_str());
	CheckSuccess(res, "Insert game record");

	PQclear(res);
}

void DatabaseWriter::StoreGame (GameData gd)
{	
	if (DEBUG)
		cout << "Writing to db..." << endl;
	if (gd.GetGameID() == DBGGAME)
		system("PAUSE");

	// Handles the players
	long playerIDs[MAXPLAYERS];
	for (int i = 0; i < gd.GetPlayerAmount(); i++)
	{
		string playerName = gd.GetPlayer(i).name;
		playerIDs[i] = GetPlayerId(playerName);
	}

	// Handles the betting rounds
	long moveSequences[4];
	for each (long& i in moveSequences)
		i = 0;
	int l = gd.GetRoundsPlayed();
	for (int gameState = PREFLOP; gameState < l; gameState++)
	{
		// Handles the moves
		long moveIDs[MAXMOVES];
		int length = gd.GetMovesLengths()[gameState];
		int playerAmount = gd.GetPlayerAmount();
		MoveData md;
		for (int i = 0; i < length; i++)
		{
			int playerPos = i % playerAmount;
			md = gd.GetMove(i, gameState);
			moveIDs[i] = InsertMove(md, playerIDs[playerPos], gd.GetPlayer(playerPos));
		}
		if (length != 0)
			moveSequences[gameState] = InsertMoveSequence(moveIDs, length, playerAmount);
	}

	// Handles the games
	InsertGame(gd, moveSequences);
}

DatabaseWriter::DatabaseWriter ()
{
	conn = ConnectDB();
	InitDB();
}