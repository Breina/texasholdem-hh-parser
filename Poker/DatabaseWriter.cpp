#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <windows.h>
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
	PQexec(conn, "ALTER SEQUENCE players_playerid_seq RESTART WITH 1");
	PQexec(conn, "ALTER SEQUENCE movesequences_movesequenceid_seq RESTART WITH 1");
	PQexec(conn, "ALTER SEQUENCE moves_moveid_seq RESTART WITH 1");
	PQexec(conn, "ALTER SEQUENCE games_game_db_id_seq RESTART WITH 1");

	playerID = 1;
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
	char user[32];
	char pass[32];
	char dbname[32];
	char host[32];
	char port[32];

	cout << "Loading database settings..." << endl;
	
	GetPrivateProfileString("DB", "user", "", user, 32, "./settings.ini");
	GetPrivateProfileString("DB", "pass", "", pass, 32, "./settings.ini");
	GetPrivateProfileString("DB", "dbname", "", dbname, 32, "./settings.ini");
	GetPrivateProfileString("DB", "host", "", host, 32, "./settings.ini");
	GetPrivateProfileString("DB", "port", "", port, 32, "./settings.ini");

	string connection = "user=";
	connection.append(user);
	connection.append(" password=");
	connection.append(pass);
	connection.append(" dbname=");
	connection.append(dbname);
	connection.append(" hostaddr=");
	connection.append(host);
	connection.append(" port=");
	connection.append(port);

	cout << "Connecting to database..." << endl;

	PGconn *conn = NULL;
	conn = PQconnectdb(connection.c_str());

	if (PQstatus(conn) != CONNECTION_OK)
	{
		printf("Connection to database failed");
		DatabaseWriter::CloseConn(conn);
		exit(1);
	}

    return conn;
}

// You do not want to open this function
void DatabaseWriter::BuildDB () 
{
	string playersTable = "";
	playersTable.append("CREATE TABLE players "				);
	playersTable.append("("									);
	playersTable.append("playerid bigserial NOT NULL,"		);
	playersTable.append("\"name\" text NOT NULL,"			);
	playersTable.append("count integer NOT NULL,"			);
	playersTable.append("vpip real,"						);
	playersTable.append("pfr real,"							);
	playersTable.append("\"3b\" real,"						);
	playersTable.append("f3 real,"							);
	playersTable.append("ag real,"							);
	playersTable.append("wtsd real,"						);
	playersTable.append("cb real,"							);
	playersTable.append("\"2b\" real,"						);
	playersTable.append("fc real,"							);
	playersTable.append("f2 real,"							);
	playersTable.append(")"									);
	playersTable.append("WITH ("							);
	playersTable.append("OIDS=FALSE"						);
	playersTable.append(");"								);
	playersTable.append("ALTER TABLE players OWNER TO postgres");

	string actionsTable = "";
	actionsTable.append("CREATE TABLE actions ");
	actionsTable.append("( ");
	actionsTable.append("actionid integer NOT NULL, ");
	actionsTable.append("\"name\" text, ");
	actionsTable.append(") ");
	actionsTable.append("WITH ( ");
	actionsTable.append("OIDS=FALSE ");
	actionsTable.append("); ");
	actionsTable.append("ALTER TABLE actions OWNER TO postgres");

	string actionsInsert = "";
	actionsInsert.append("INSERT INTO actions VALUES ");
	actionsInsert.append("('0', 'SKIP'), ");
	actionsInsert.append("('1', 'FOLD'), ");
	actionsInsert.append("('2', 'CALL'), ");
	actionsInsert.append("('3', 'CHECK'), ");
	actionsInsert.append("('4', 'RAISE')");
	
	string movesTable = "";
	movesTable.append("CREATE TABLE moves ");
	movesTable.append("( ");
	movesTable.append("moveid bigserial NOT NULL, ");
	movesTable.append("\"action\" smallint NOT NULL, ");
	movesTable.append("pot integer NOT NULL, ");
	movesTable.append("ocall integer, ");
	movesTable.append("stack integer, ");
	movesTable.append("bet integer, ");
	movesTable.append("playerid bigint NOT NULL, ");
	movesTable.append("card1 character(2), ");
	movesTable.append("card2 character(2), ");
	movesTable.append("currentbet integer, ");
	movesTable.append(") ");
	movesTable.append("WITH ( ");
	movesTable.append("OIDS=FALSE ");
	movesTable.append("); ");
	movesTable.append("ALTER TABLE moves OWNER TO postgres");

	string moveSequencesTable = "";
	moveSequencesTable.append("CREATE TABLE movesequences ");
	moveSequencesTable.append("( ");
	moveSequencesTable.append("movesequenceid bigserial NOT NULL, ");
	moveSequencesTable.append("p0m0 bigint, ");
	moveSequencesTable.append("p1m0 bigint, ");
	moveSequencesTable.append("p2m0 bigint, ");
	moveSequencesTable.append("p3m0 bigint, ");
	moveSequencesTable.append("p4m0 bigint, ");
	moveSequencesTable.append("p5m0 bigint, ");
	moveSequencesTable.append("p6m0 bigint, ");
	moveSequencesTable.append("p7m0 bigint, ");
	moveSequencesTable.append("p8m0 bigint, ");
	moveSequencesTable.append("p9m0 bigint, ");
	moveSequencesTable.append("p0m1 bigint, ");
	moveSequencesTable.append("p1m1 bigint, ");
	moveSequencesTable.append("p2m1 bigint, ");
	moveSequencesTable.append("p3m1 bigint, ");
	moveSequencesTable.append("p4m1 bigint, ");
	moveSequencesTable.append("p5m1 bigint, ");
	moveSequencesTable.append("p6m1 bigint, ");
	moveSequencesTable.append("p7m1 bigint, ");
	moveSequencesTable.append("p8m1 bigint, ");
	moveSequencesTable.append("p9m1 bigint, ");
	moveSequencesTable.append("p0m2 bigint, ");
	moveSequencesTable.append("p1m2 bigint, ");
	moveSequencesTable.append("p2m2 bigint, ");
	moveSequencesTable.append("p3m2 bigint, ");
	moveSequencesTable.append("p4m2 bigint, ");
	moveSequencesTable.append("p5m2 bigint, ");
	moveSequencesTable.append("p6m2 bigint, ");
	moveSequencesTable.append("p7m2 bigint, ");
	moveSequencesTable.append("p8m2 bigint, ");
	moveSequencesTable.append("p9m2 bigint, ");
	moveSequencesTable.append("p0m3 bigint, ");
	moveSequencesTable.append("p1m3 bigint, ");
	moveSequencesTable.append("p2m3 bigint, ");
	moveSequencesTable.append("p3m3 bigint, ");
	moveSequencesTable.append("p4m3 bigint, ");
	moveSequencesTable.append("p5m3 bigint, ");
	moveSequencesTable.append("p6m3 bigint, ");
	moveSequencesTable.append("p7m3 bigint, ");
	moveSequencesTable.append("p8m3 bigint, ");
	moveSequencesTable.append("p9m3 bigint, ");
	moveSequencesTable.append("p0m4 bigint, ");
	moveSequencesTable.append("p1m4 bigint, ");
	moveSequencesTable.append("p2m4 bigint, ");
	moveSequencesTable.append("p3m4 bigint, ");
	moveSequencesTable.append("p4m4 bigint, ");
	moveSequencesTable.append("p5m4 bigint, ");
	moveSequencesTable.append("p6m4 bigint, ");
	moveSequencesTable.append("p7m4 bigint, ");
	moveSequencesTable.append("p8m4 bigint, ");
	moveSequencesTable.append("p9m4 bigint, ");
	moveSequencesTable.append("p0m5 bigint, ");
	moveSequencesTable.append("p1m5 bigint, ");
	moveSequencesTable.append("p2m5 bigint, ");
	moveSequencesTable.append("p3m5 bigint, ");
	moveSequencesTable.append("p4m5 bigint, ");
	moveSequencesTable.append("p5m5 bigint, ");
	moveSequencesTable.append("p6m5 bigint, ");
	moveSequencesTable.append("p7m5 bigint, ");
	moveSequencesTable.append("p8m5 bigint, ");
	moveSequencesTable.append("p9m5 bigint, ");
	moveSequencesTable.append(") ");
	moveSequencesTable.append("WITH ( ");
	moveSequencesTable.append("OIDS=FALSE ");
	moveSequencesTable.append("); ");
	moveSequencesTable.append("ALTER TABLE movesequences OWNER TO postgres");

	string gamesTable = "";
	gamesTable.append("CREATE TABLE games ");
	gamesTable.append("( ");
	gamesTable.append("game_db_id bigserial NOT NULL, ");
	gamesTable.append("gameid text NOT NULL, ");
	gamesTable.append("sb integer NOT NULL, ");
	gamesTable.append("bb integer NOT NULL, ");
	gamesTable.append("limittype text, ");
	gamesTable.append("gametype text, ");
	gamesTable.append("playeramount integer NOT NULL, ");
	gamesTable.append("preflop bigint, ");
	gamesTable.append("flop bigint, ");
	gamesTable.append("turn bigint, ");
	gamesTable.append("river bigint, ");
	gamesTable.append(") ");
	gamesTable.append("WITH ( ");
	gamesTable.append("OIDS=FALSE ");
	gamesTable.append("); ");
	gamesTable.append("ALTER TABLE games OWNER TO postgres");

	string logic = "";
	logic.append("CREATE TYPE t_player AS ( ");
	logic.append("id bigint, ");
	logic.append("card1 character(2), ");
	logic.append("card2 character(2) ");
	logic.append("); ");
	logic.append(" ");
	logic.append("CREATE TYPE t_move AS ( ");
	logic.append("actn smallint, ");
	logic.append("pot integer, ");
	logic.append("bet integer, ");
	logic.append("tocall integer, ");
	logic.append("currentbet integer, ");
	logic.append("stack integer ");
	logic.append("); ");
	logic.append(" ");
	logic.append("CREATE TYPE t_round AS ( ");
	logic.append("movelist t_move[], ");
	logic.append("length smallint ");
	logic.append("); ");
	logic.append("	 ");
	logic.append(" ");
	logic.append("CREATE TYPE t_game AS ( ");
	logic.append("gameid text, ");
	logic.append("sb integer, ");
	logic.append("bb integer, ");
	logic.append("limittype text, ");
	logic.append("gametype text, ");
	logic.append("playeramount integer, ");
	logic.append(" ");
	logic.append("players t_player[], ");
	logic.append("rounds t_round[] ");
	logic.append("); ");
	logic.append(" ");
	logic.append("CREATE FUNCTION importgames(g t_game[]) RETURNS boolean AS $$ ");
	logic.append("DECLARE ");
	logic.append("i integer; ");
	logic.append("BEGIN ");
	logic.append("RETURN true; ");
	logic.append("END; ");
	logic.append("$$ LANGUAGE plpgsql; ");
	
	PQexec(conn, playersTable.c_str());
	PQexec(conn, actionsTable.c_str());
	PQexec(conn, actionsInsert.c_str());
	PQexec(conn, movesTable.c_str());
	PQexec(conn, moveSequencesTable.c_str());
	PQexec(conn, gamesTable.c_str());
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

void DatabaseWriter::StoreGame (GameData gd)
{
	for (int i = 0; i < gd.GetPlayerAmount(); i++)
	{
		std::unordered_map<std::string, playerDat>::iterator it = players->find(gd.GetPlayerName(i));
		if (it == players->end()) // player not found
		{
			playerID++;
			playerDat p;
			p.count = 1;
			p.id = playerID;
			players->emplace(std::pair<std::string, playerDat>(gd.GetPlayerName(i), p));
		}
		else
		{
			it->second.count++;
		}
	}


}

void DatabaseWriter::PopulatePlayerList ()
{
	PGresult *res = PQexec(conn, "SELECT playerid, name FROM players");
	CheckSuccess(res, "Obtaining players");

	int amount = PQntuples(res); // Future error: PQntuples returns an integer, while we database's pk is bigint (or long)
	for (int i = 0; i < amount; i++)
	{
		playerDat p;
		p.id	= atoi(PQgetvalue(res, i, 0));
		p.count = 1;
		players->emplace(std::pair<std::string, playerDat>((string) PQgetvalue(res, i, 1), p));
	}
	playerID = atoi(PQgetvalue(res, amount - 1, 0));
	
	PQclear(res);
}

DatabaseWriter::DatabaseWriter (bool cachePlayers)
{
	conn = ConnectDB();
	if (!cachePlayers) return;

	players = new unordered_map<std::string, playerDat>();
	players->rehash(100000);
}