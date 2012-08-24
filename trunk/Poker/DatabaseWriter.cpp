#include "stdafx.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>
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

const string USER = "postgres";
const string PASS = "superman";
const string DBNAME="poker";
const string HOST = "127.0.0.1";
const string PORT = "5432";

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
	
	GetPrivateProfileString("DB", "user", USER.c_str(), user, 32, "./settings.ini");
	GetPrivateProfileString("DB", "pass", PASS.c_str(), pass, 32, "./settings.ini");
	GetPrivateProfileString("DB", "dbname", DBNAME.c_str(), dbname, 32, "./settings.ini");
	GetPrivateProfileString("DB", "host", HOST.c_str(), host, 32, "./settings.ini");
	GetPrivateProfileString("DB", "port", PORT.c_str(), port, 32, "./settings.ini");

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
	playersTable.append("CONSTRAINT playerid_pk PRIMARY KEY (playerid),");
	playersTable.append("CONSTRAINT players_name_unique UNIQUE (name)");
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
	actionsTable.append("CONSTRAINT actions_actionid_pk PRIMARY KEY (actionid) ");
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
	movesTable.append("CONSTRAINT moveid_pk PRIMARY KEY (moveid), ");
	movesTable.append("CONSTRAINT action_fk FOREIGN KEY (\"action\") ");
    movesTable.append("REFERENCES actions (actionid) MATCH SIMPLE ");
    movesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION ");
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
	moveSequencesTable.append("CONSTRAINT movesequenceid_pk PRIMARY KEY (movesequenceid), ");
	moveSequencesTable.append("CONSTRAINT p0m0_pk FOREIGN KEY (p0m0) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p0m1_pk FOREIGN KEY (p0m1) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p0m2_pk FOREIGN KEY (p0m2) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p0m3_pk FOREIGN KEY (p0m3) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p0m4_pk FOREIGN KEY (p0m4) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p0m5_pk FOREIGN KEY (p0m5) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p1m0_pk FOREIGN KEY (p1m0) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p1m1_pk FOREIGN KEY (p1m1) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p1m2_pk FOREIGN KEY (p1m2) ");
	moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
	moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p1m3_pk FOREIGN KEY (p1m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p1m4_pk FOREIGN KEY (p1m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p1m5_pk FOREIGN KEY (p1m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p2m0_pk FOREIGN KEY (p2m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p2m1_pk FOREIGN KEY (p2m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p2m2_pk FOREIGN KEY (p2m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p2m3_pk FOREIGN KEY (p2m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p2m4_pk FOREIGN KEY (p2m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p2m5_pk FOREIGN KEY (p2m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p3m0_pk FOREIGN KEY (p3m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p3m1_pk FOREIGN KEY (p3m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p3m2_pk FOREIGN KEY (p3m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p3m3_pk FOREIGN KEY (p3m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p3m4_pk FOREIGN KEY (p3m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p3m5_pk FOREIGN KEY (p3m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p4m0_pk FOREIGN KEY (p4m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p4m1_pk FOREIGN KEY (p4m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p4m2_pk FOREIGN KEY (p4m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p4m3_pk FOREIGN KEY (p4m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p4m4_pk FOREIGN KEY (p4m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p4m5_pk FOREIGN KEY (p4m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p5m0_pk FOREIGN KEY (p5m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p5m1_pk FOREIGN KEY (p5m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p5m2_pk FOREIGN KEY (p5m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p5m3_pk FOREIGN KEY (p5m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p5m4_pk FOREIGN KEY (p5m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p5m5_pk FOREIGN KEY (p5m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p6m0_pk FOREIGN KEY (p6m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p6m1_pk FOREIGN KEY (p6m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p6m2_pk FOREIGN KEY (p6m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p6m3_pk FOREIGN KEY (p6m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p6m4_pk FOREIGN KEY (p6m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p6m5_pk FOREIGN KEY (p6m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p7m0_pk FOREIGN KEY (p7m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p7m1_pk FOREIGN KEY (p7m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p7m2_pk FOREIGN KEY (p7m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p7m3_pk FOREIGN KEY (p7m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p7m4_pk FOREIGN KEY (p7m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p7m5_pk FOREIGN KEY (p7m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p8m0_pk FOREIGN KEY (p8m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p8m1_pk FOREIGN KEY (p8m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p8m2_pk FOREIGN KEY (p8m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p8m3_pk FOREIGN KEY (p8m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p8m4_pk FOREIGN KEY (p8m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p8m5_pk FOREIGN KEY (p8m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p9m0_pk FOREIGN KEY (p9m0) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p9m1_pk FOREIGN KEY (p9m1) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p9m2_pk FOREIGN KEY (p9m2) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p9m3_pk FOREIGN KEY (p9m3) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p9m4_pk FOREIGN KEY (p9m4) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	moveSequencesTable.append("CONSTRAINT p9m5_pk FOREIGN KEY (p9m5) ");
    moveSequencesTable.append("REFERENCES moves (moveid) MATCH SIMPLE ");
    moveSequencesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION ");
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
	gamesTable.append("CONSTRAINT games_gameid_pk PRIMARY KEY (game_db_id), ");
	gamesTable.append("CONSTRAINT games_flop_fk FOREIGN KEY (flop) ");
    gamesTable.append("REFERENCES movesequences (movesequenceid) MATCH SIMPLE ");
    gamesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	gamesTable.append("CONSTRAINT games_preflop_fk FOREIGN KEY (preflop) ");
    gamesTable.append("REFERENCES movesequences (movesequenceid) MATCH SIMPLE ");
    gamesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	gamesTable.append("CONSTRAINT games_river_fk FOREIGN KEY (river) ");
    gamesTable.append("REFERENCES movesequences (movesequenceid) MATCH SIMPLE ");
    gamesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION, ");
	gamesTable.append("CONSTRAINT games_turn_fk FOREIGN KEY (turn) ");
    gamesTable.append("REFERENCES movesequences (movesequenceid) MATCH SIMPLE ");
    gamesTable.append("ON UPDATE NO ACTION ON DELETE NO ACTION ");
	gamesTable.append(") ");
	gamesTable.append("WITH ( ");
	gamesTable.append("OIDS=FALSE ");
	gamesTable.append("); ");
	gamesTable.append("ALTER TABLE games OWNER TO postgres");

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

long DatabaseWriter::GetPlayerId (string& playerName)
{
	map<string, long>::iterator it = playerList->find(playerName.c_str());
	if (it != playerList->end())
	{
		long ret = it->second;
		return ret;
	}/*
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
	{*/
	long ret = InsertPlayer(playerName);
	playerList->insert(pair<string, long>(playerName, ret));
	//}

	//PQclear(res); // THIS FUCKING PIECE OF SHIT CODE WAS MISSING, CAUSING A MEMORY LEAK HOLDING THIS PROJECT BACK FOR WEEKS. FUKA-U CODE! FUKA YU WELL!!

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
	playerList = new map<string, long>();
}