// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <string>
#include <iostream>
#include "GameData.h"

using namespace std;

const string INDENT =	" -";

const string FOLDS =	"folds";
const string RAISES =	"raises";
const string CALLS =	"calls";
const string CHECKS =	"checks";
const string ADDS =		"adds";
const string BETS =		"bets";
const string DEALT =	"Dealt to";
const string HAS =		"has";
const string NEXTROUND=	"*** ";
const string UNCALLED =	"Uncalled bet";
const string COLLECTED=	"ollected ";
const string SEAT =		"Seat ";
const string INCHIPS =	" in chips";

const string GAMESTATE[] = {"Preflop", "Flop   ", "Turn   ", "River  "};

const int PREFLOP =	0;
const int FLOP =	1;
const int TURN =	2;
const int RIVER =	3;
const int SHOWDOWN=	4;

const char SKIP = 'x';
const char FOLD = 'f';
const char CALL = 'c';
const char RAISE= 'r';



// Skips characters until it finds the target character, stops at the character itself
void GameData::skipToChar (string& source, int& pos, char c)
{
	while (source[pos] != c)
		pos++;
}
// Skips characters until it finds the target character, stops at the character after the target
void GameData::skipOverChar (string& source, int& pos, char c)
{
	skipToChar(source, pos, c);
	pos++;
}
// Copies all characters to the destination string until it finds the target character
void GameData::writeToChar (string& source, string& dest, int& pos, char c)
{
	string s;
	while (source[pos] != c)
	{
		s = source[pos];
		dest.append(s);
		pos++;
	}
}
// Customized version of writeToChar, because names can have stuff like (, ) and $
void GameData::parsePlayerName (string& source, string& dest, int& pos)
{
	string s;
	while (true)
	{
		writeToChar(source, dest, pos, '$');
		int p = pos;
		skipToChar(source, p, ' ');
		if (checkString(source, p, INCHIPS)) // If the $ was not in the player name; done
			break;
		s = source[pos];
		dest.append(s);						// Else it was part of the player name; add it and continue
		pos++;
	}
	trimStringEnd(dest, 2);
}
// Skips all characters until the next line, stops at the first character after the new line
void GameData::skipToNextLine(string& source, int& pos)
{
	skipOverChar(source, pos, '\n');
}

// Skips al characters until the next game, stops at the first character of the new game
bool GameData::skipToNextGame(string& source, int& pos)
{
	if (gameID == "75956073148")
	{
		cout << endl;
	}
	bool ret = false;
	bool currentNL = false;
	bool previousNL = false;
	bool foundGap = false;
	for (; pos < source.length(); pos++)
	{
		currentNL = (source[pos] == '\n');
		if (currentNL && previousNL)
			foundGap = true;
		if (foundGap && !currentNL)
			return ret;
		previousNL = currentNL;
	}
	ret = true;
	return ret;
}
// Just jump a couple characters, either positive or negative
void GameData::jumpPos (int& pos, int amount)
{
	pos += amount;
}
// Shortens a string by the set amount
void GameData::trimStringEnd (string& s, int amount)
{
	s.resize(s.length() - amount);
}
// Checks if the next characters in the source string are identical to the match string
bool GameData::checkString(string& source, int& pos, string match)
{
	int l = match.length();
	bool ret = false;
	for (int i = 0; i < l; i++)
		if (source[pos + i] != match[i])
			return ret;
	ret = true;
	return ret;
}
// Reads the player name and returns the position the player position in the order in which they were first read.
// Its intended use is to find out who's the sb, to re-order the player list, so it's only used once.
int GameData::readPlayerName(string& source, int& pos, string players[])
{
	int i;
	for (i = 0; i < pAmount; i++)
	{
		if (checkString(source, pos, players[i]))
			return i;
	}
	i = -1;
	return i;
}
// Advances the player position, and makes sure to handle overflow
void GameData::advancePlayerPosition(int& p)
{
	p++;
	if (p == pAmount) p = 0; // You can write it like p = (p + 1) % pAmount, but it will take more CPU cycles
}
// Re-orders the player positions to absolute positions, so sb will be first
void GameData::setPlayerPositions(string players[], int& p, int pPosition[])
{
	//int p = pAmount - first;
	for (int i = 0; i < pAmount; i++) {
		pPosition[i] = p;
		advancePlayerPosition(p);
	}
}
// Converts a string to pennies, so we won't have to deal with floats
int GameData::strToPennies(string str)
{
	int ret;
	int l = str.length();
	for (int i = 0; i < l; i++)
	{
		if (str[i] == '.') 
		{
			ret = atoi(str.replace(i, 1, "").c_str());
			return ret;
		}
	}
	ret = atoi(str.c_str()) * 100;
	return ret;
}
// Sets up the player structures, which will be used to store moves
void GameData::setupPlayerStructs(string players[], string pCash[], int pPosition[])
{
	int position;
	for (int i = 0; i < pAmount; i++)
	{
		position = pPosition[i];
		playerData[i].name = players[position];
		playerData[i].money = strToPennies(pCash[position]);
	} // end for
}
// Adds an action to all players. For one player, it is stored in its structure
void GameData::addAction(int& pPos, char action, int gameState, string& movesBuffer, int& movesCounter)
{
	movesBuffer[movesCounter] = action;
	movesCounter++;
	if (action != SKIP)
	{
		playerData[pPos].moves[gameState].append(movesBuffer);
		playerData[pPos].moves[gameState].append("\n");
	}
	advancePlayerPosition(pPos);
	cout << action;
}
// Main function that parses through the entire source file
void GameData::parseAll(string& source, int& pos)
{
	skipOverChar(source, pos, '#');				//
	writeToChar(source, gameID, pos, ':');		//
	
	cout << "Parsing game #" << gameID << ":";

	skipOverChar(source, pos, 'm');				//
	jumpPos(pos, 1);							//
	writeToChar(source, limitType, pos, '(');	//
	trimStringEnd(limitType, 1);				//
	jumpPos(pos, 2);							//
	writeToChar(source, sb, pos, '/');			//
	jumpPos(pos, 2);							//
	writeToChar(source, bb, pos, ' ');			//
	jumpPos(pos, 3);							//
	skipOverChar(source, pos, '\'');			//
	skipOverChar(source, pos, '\'');			//
	skipOverChar(source, pos, ' ');				//
	writeToChar(source, gameType, pos, ' ');	//
	skipToNextLine(source, pos);				//

	{
		string players	[MAXPLAYERS];			// We don't know the amount of players yet, this array is temporarily
		string pCash	[MAXPLAYERS];
		{
			int i = 0;
			while (checkString(source, pos, SEAT))
			{
				jumpPos(pos, 8);					// Seat 1: 
				parsePlayerName(source, players[i], pos);
				jumpPos(pos, 1);					// ($
				writeToChar(source, pCash[i],
					pos, ' ');						// 68.30
				skipToNextLine(source, pos);
				i++;
			} // end while
			pAmount = i;							// Player amount
		} // end i
		int playerPos = readPlayerName(source, pos, players); // Relative to the order of how the players were parsed, becomes absolute after setPlayerPositions() is used
		int pPosition[MAXPLAYERS];
		setPlayerPositions(players, playerPos, pPosition);		// Orders the players correctly, so 0 = sb, 1 = bb, ..
		setupPlayerStructs(players, pCash, pPosition);
	} // end players[], pCash[], playerPos, pPosition[]
	skipToNextLine(source, pos);				// Small blind				TODO: Failsafe
	skipToNextLine(source, pos);				// Big blind				TODO: Failsafe
	skipToNextLine(source, pos);				// *** HOLE CARDS ***		TODO: Failsafe
	{
		int playerPos = 2;						// Skipping the sb and bb, sb and bb is a rule, not a player's strategy, so no point in including them
		while (checkString(source, pos, DEALT))
		{
			skipOverChar(source, pos, '[');			// Dealt to Hero [
			writeToChar(source, playerData[playerPos].card1, pos, ' ');	// Jc
			jumpPos(pos, 1);						//  
			writeToChar(source, playerData[playerPos].card2, pos, ']');	// Kd
			skipToNextLine(source, pos);
		} // end while
		{
			int gameState = 0;
			while (gameState < SHOWDOWN)
			{
				string movesBuffer;
				movesBuffer.resize(MAXMOVES);

				cout << endl << INDENT << "" << GAMESTATE[gameState] << ": ";

				int movesCounter = 0;
				while (!checkString(source, pos, NEXTROUND))
				{
					if (!checkString(source, pos, UNCALLED))		// If it's not something that we don't handle yet
																	// TODO: When handling bet amounts, take this into account
					{
						if (!checkString(source, pos, playerData[playerPos].name))
						{
							addAction(playerPos, SKIP , gameState, movesBuffer, movesCounter); // This player didn't make a move, because he folded earlier or something like that
							continue;
						}
						jumpPos(pos, playerData[playerPos].name.length() + 2);
						if (checkString(source, pos, FOLDS))
						{
							addAction(playerPos, FOLD , gameState, movesBuffer, movesCounter);
						}
						else if (checkString(source, pos, CALLS) ||
								 checkString(source, pos, CHECKS))
						{
							addAction(playerPos, CALL , gameState, movesBuffer, movesCounter);
						}
						else if (checkString(source, pos, RAISES) ||
								 checkString(source, pos, ADDS) ||
								 checkString(source, pos, BETS))
						{
							addAction(playerPos, RAISE, gameState, movesBuffer, movesCounter);
						}
						else if (checkString(source, pos, COLLECTED))
						{
							gameState = SHOWDOWN;
							cout << "W" << endl;
							break;
						}
						else
						{
							// Ignore action
						}
					}
					skipToNextLine(source, pos);
				} // end while
				gameState++;
				skipToNextLine(source, pos);
				playerPos = 0;
			} // end while
		} // end block
	} // end playerPos
	cout << endl;

	last = skipToNextGame(source, pos);
}

GameData::GameData (string& source, int& pos)
{
	parseAll(source, pos);
}