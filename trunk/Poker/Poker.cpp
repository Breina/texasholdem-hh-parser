// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include "FileHandler.h"

using namespace std;

const int MAXPLAYERS = 10;
const int MAXMOVES = 16;
const string HANDHISTORYPATH = "C:\\Users\\CX\\Desktop\\hh.txt";

const string FOLDS =	"folds";
const string RAISES =	"raises to";
const string CALLS =	"calls";
const string ADDS =		"adds";
const string BETS =		"bets";
const string DEALT =	"Dealt to";
const string HAS =		"has";
const string NEXTROUND=	"*** ";

const int PREFLOP =	0;
const int FLOP =	1;
const int TURN =	2;
const int RIVER =	3;

const char SKIP = 'x';
const char FOLD = 'f';
const char CALL = 'c';
const char RAISE= 'r';

struct PlayerData {
	string name;
	string card1, card2;
	int money;		// At start of the game
	string moves[4];
};

string gameID;		// Jha de ID eh
string gameType;	// 6 max, heads up, ..
string sb, bb;		// Small blind, big blind
string limitType;	// No limit, fixed limit
string players[MAXPLAYERS]; // ...
string pCash[MAXPLAYERS];	// Geld wa die hebbe
int pPosition[MAXPLAYERS];	// Positie, 0=sb, 1=bb, .. pAmount-1=dealer
bool pActive[MAXPLAYERS] = {true};	// Is speler actief?
int pAmount;		// Aantal spelers
PlayerData playerData[MAXPLAYERS];	// Player structs
char movesBuffer[MAXMOVES];
int movesCounter;	// Hoeveel moves der zijn geweest per round
int gameState;		// 0 = preflop, 1 = flop, 2 = turn, 3 = river

void skipToChar (string& source, int& pos, char c)
{
	while (source[pos] != c)
		pos++;
}

void skipOverChar (string& source, int& pos, char c)
{
	skipToChar(source, pos, c);
	pos++;
}

void writeToChar (string& source, string& dest, int& pos, char c)
{
	string s;
	while (source[pos] != c)
	{
		s = source[pos];
		dest.append(s);
		pos++;
	}
}

void skipToNextLine(string& source, int& pos)
{
	skipOverChar(source, pos, '\n');
}

void jumpPos (int& pos, int amount)
{
	pos += amount;
}

void trimStringEnd (string& s, int amount)
{
	s.resize(s.length() - amount);
}

bool checkString(string& source, int& pos, string match)
{
	int l = match.length();
	//cout << "Checking: " << source.substr(pos, l) << " - " << match << endl;
	for (int i = 0; i < l; i++)
		if (source[pos + i] != match[i])
			return false;
	//cout << "Returned true." << endl;
	return true;
}

int readPlayerName(string& source, int& pos, string players[])
{
	for (int i = 0; i < pAmount; i++)
	{
		if (checkString(source, pos, players[i]))
			return i;
	}
	return -1;
}

void advancePlayerPosition(int& p)
{
	p++;
	if (p == pAmount) p = 0; // Kunt ge ook schrijve as p = (p + 1) % pAmount, ma door de deling kost da meer CPU cycles denk ik
}

void setPlayerPositions(string players[], int first)
{
	int p = pAmount - first;
	for (int i = 0; i < pAmount; i++) {
		pPosition[i] = p;
		advancePlayerPosition(p);
	}
}

int strToPennies(string str)
{
	int l = str.length();
	for (int i = 0; i < l; i++)
	{
		if (str[i] == '.') 
		{
			return atoi(str.replace(i, 1, "").c_str());
			break;
		}
	}
	return atoi(str.c_str()) * 100;
}

void setupPlayerStructs(string players[])
{
	int position;
	for (int i = 0; i < pAmount; i++)
	{
		position = pPosition[i];
		playerData[i].name = players[position];
		playerData[i].money = strToPennies(pCash[position]);
	}
}

void addAction(int& pPos, char action)
{
	movesBuffer[movesCounter] = action;
	movesCounter++;
	if (action != SKIP)
	{
		playerData[pPos].moves[gameState].append(movesBuffer);
		playerData[pPos].moves[gameState].append("\n");
	}
	advancePlayerPosition(pPos);
	cout << " Action: " << action << endl;
}

void parseAll(string& source)
{
	int pos = 0;
	skipOverChar(source, pos, '#');				// Full Tilt Poker Game #
	writeToChar(source, gameID, pos, ':');		// <gameID>
	skipOverChar(source, pos, '(');				// : Table Stardust (
	writeToChar(source, gameType, pos, ')');	// <gameType>
	jumpPos(pos, 5);							// ) - $
	writeToChar(source, sb, pos, '/');			// <sb>
	jumpPos(pos, 2);							// /$
	writeToChar(source, bb, pos, ' ');			// <bb>
	jumpPos(pos, 3);							//  - 
	writeToChar(source, limitType, pos, 'H');	// No Limit 
	trimStringEnd(limitType, 1);				// No Limit
	skipToNextLine(source, pos);				// Hold'em - 15:41:10 ET - 2008/08/21
	{
		int i = 0;
		while (source[pos] == 'S')
		{
			jumpPos(pos, 8);					// Seat 1: 
			writeToChar(source, players[i],
				pos, '(');						// UOFalcan 
			trimStringEnd(players[i], 1);		// UOFalcan
			jumpPos(pos, 2);					// ($
			writeToChar(source, pCash[i],
				pos, ')');						// 68.30
			skipToNextLine(source, pos);
			i++;
		}
		pAmount = i;							// Player amount
	}
	int playerPos = readPlayerName(source, pos, players);	// Relatief tov de volgorde van de seats, gaat gerecycleerd worde absoluut (0 = sb)
	setPlayerPositions(players, playerPos);		// Fills turn values
	skipToNextLine(source, pos);				// Small blind				TODO: Failsafe
	skipToNextLine(source, pos);				// Big blind				TODO: Failsafe
	skipToNextLine(source, pos);				// Dealer button			TODO: Failsafe
	skipToNextLine(source, pos);				// *** HOLE CARDS ***		TODO: Failsafe
	playerPos = 0;								// Absolute positie (0 = sb), 2 is na de bb
	addAction(playerPos, RAISE);				// Manual sb
	addAction(playerPos, RAISE);				// Manual bb
	setupPlayerStructs(players);
	{
		int i = 0;														//	TODO: Recykel playerPos ipv i
		while (checkString(source, pos, DEALT))
		{
			skipOverChar(source, pos, '[');			// Dealt to Hero [
			writeToChar(source, playerData[i].card1, pos, ' ');	// Jc
			jumpPos(pos, 1);						//  
			writeToChar(source, playerData[i].card2, pos, ']');	// Kd
			skipToNextLine(source, pos);
		}
	}
	gameState = 0;
	while (gameState != 4)
	{
		while (!checkString(source, pos, NEXTROUND))
		{
			cout << "Reading " << playerData[playerPos].name << "'s data.";
			if (!checkString(source, pos, playerData[playerPos].name))
			{
				addAction(playerPos, SKIP);
				continue;
			}
			jumpPos(pos, playerData[playerPos].name.length() + 1);
			if (checkString(source, pos, FOLDS))
			{
				addAction(playerPos, FOLD);
			}
			else if (checkString(source, pos, CALLS))
			{
				addAction(playerPos, CALL);
			}
			else if (checkString(source, pos, RAISES) ||
					 checkString(source, pos, ADDS) ||
					 checkString(source, pos, BETS))
			{
				addAction(playerPos, RAISE);
			}
			else
			{
				cout << " Unrecognized action: " << source.substr(pos, 15) << "..." << endl;
			}
			skipToNextLine(source, pos);
		}
		gameState++;
		cout << "*** Advanced gamestate ***" << endl;
		skipToNextLine(source, pos);
		playerPos = 0;
	}
	
}

void parseFile (string path)
{
	string source;
	getFile(source, path);
	parseAll(source);
}

int _tmain (int argc, _TCHAR* argv[])
{
	parseFile(HANDHISTORYPATH);
	
	system ("pause");
	return 0;
}