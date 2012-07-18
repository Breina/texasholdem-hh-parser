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
void GameData::SkipToChar (string& source, int& pos, char c)
{
	while (source[pos] != c)
		pos++;
}
// Skips characters until it finds the target character, stops at the character after the target
void GameData::SkipOverChar (string& source, int& pos, char c)
{
	SkipToChar(source, pos, c);
	pos++;
}
// Copies all characters to the destination string until it finds the target character
void GameData::WriteToChar (string& source, string& dest, int& pos, char c)
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
void GameData::ParsePlayerName (string& source, string& dest, int& pos)
{
	string s;
	while (true)
	{
		WriteToChar(source, dest, pos, '$');
		int p = pos;
		SkipToChar(source, p, ' ');
		if (CheckString(source, p, INCHIPS)) // If the $ was not in the player name; done
			break;
		s = source[pos];
		dest.append(s);						// Else it was part of the player name; add it and continue
		pos++;
	}
	TrimStringEnd(dest, 2);
}
// Skips all characters until the next line, stops at the first character after the new line
void GameData::SkipToNextLine(string& source, int& pos)
{
	SkipOverChar(source, pos, '\n');
}

// Skips al characters until the next game, stops at the first character of the new game
bool GameData::SkipToNextGame(string& source, int& pos)
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
void GameData::JumpPos (int& pos, int amount)
{
	pos += amount;
}
// Shortens a string by the set amount
void GameData::TrimStringEnd (string& s, int amount)
{
	s.resize(s.length() - amount);
}
// Checks if the next characters in the source string are identical to the match string
bool GameData::CheckString(string& source, int& pos, string match)
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
int GameData::ReadPlayerName(string& source, int& pos, string players[])
{
	int i;
	for (i = 0; i < pAmount; i++)
	{
		if (CheckString(source, pos, players[i]))
			return i;
	}
	i = -1;
	return i;
}
// Advances the player position, and makes sure to handle overflow
void GameData::AdvancePlayerPosition(int& p)
{
	p++;
	if (p == pAmount) p = 0; // You can write it like p = (p + 1) % pAmount, but it will take more CPU cycles
}
// Re-orders the player positions to absolute positions, so sb will be first
void GameData::SetPlayerPositions(string players[], int& p, int pPosition[])
{
	//int p = pAmount - first;
	for (int i = 0; i < pAmount; i++) {
		pPosition[i] = p;
		AdvancePlayerPosition(p);
	}
}
// Converts a string to pennies, so we won't have to deal with floats
int GameData::StrToPennies(string str)
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
void GameData::SetupPlayerStructs(string players[], string pCash[], int pPosition[])
{
	int position;
	for (int i = 0; i < pAmount; i++)
	{
		position = pPosition[i];
		playerData[i].name = players[position];
		playerData[i].money = StrToPennies(pCash[position]);
	} // end for
}
// Adds an action to all players. For one player, it is stored in its structure
void GameData::AddAction(int& pPos, char action, int gameState, string& movesBuffer, int& movesCounter)
{
	movesBuffer[movesCounter] = action;
	movesCounter++;
	if (action != SKIP)
	{
		playerData[pPos].moves[gameState].append(movesBuffer);
		playerData[pPos].moves[gameState].append("\n");
	}
	AdvancePlayerPosition(pPos);
	cout << action;
}
// Main function that parses through the entire source file
void GameData::ParseAll(string& source, int& pos)
{
	SkipOverChar(source, pos, '#');				//
	WriteToChar(source, gameID, pos, ':');		//
	
	cout << "Parsing game #" << gameID << ":";

	SkipOverChar(source, pos, 'm');				//
	JumpPos(pos, 1);							//
	WriteToChar(source, limitType, pos, '(');	//
	TrimStringEnd(limitType, 1);				//
	JumpPos(pos, 2);							//
	WriteToChar(source, sb, pos, '/');			//
	JumpPos(pos, 2);							//
	WriteToChar(source, bb, pos, ' ');			//
	JumpPos(pos, 3);							//
	SkipOverChar(source, pos, '\'');			//
	SkipOverChar(source, pos, '\'');			//
	SkipOverChar(source, pos, ' ');				//
	WriteToChar(source, gameType, pos, ' ');	//
	SkipToNextLine(source, pos);				//

	{
		string players	[MAXPLAYERS];			// We don't know the amount of players yet, this array is temporarily
		string pCash	[MAXPLAYERS];
		{
			int i = 0;
			while (CheckString(source, pos, SEAT))
			{
				JumpPos(pos, 8);					// Seat 1: 
				ParsePlayerName(source, players[i], pos);
				JumpPos(pos, 1);					// ($
				WriteToChar(source, pCash[i],
					pos, ' ');						// 68.30
				SkipToNextLine(source, pos);
				i++;
			} // end while
			pAmount = i;							// Player amount
		} // end i
		int playerPos = ReadPlayerName(source, pos, players); // Relative to the order of how the players were parsed, becomes absolute after setPlayerPositions() is used
		int pPosition[MAXPLAYERS];
		SetPlayerPositions(players, playerPos, pPosition);		// Orders the players correctly, so 0 = sb, 1 = bb, ..
		SetupPlayerStructs(players, pCash, pPosition);
	} // end players[], pCash[], playerPos, pPosition[]
	SkipToNextLine(source, pos);				// Small blind				TODO: Failsafe
	SkipToNextLine(source, pos);				// Big blind				TODO: Failsafe
	SkipToNextLine(source, pos);				// *** HOLE CARDS ***		TODO: Failsafe
	{
		int playerPos = 2;						// Skipping the sb and bb, sb and bb is a rule, not a player's strategy, so no point in including them
		while (CheckString(source, pos, DEALT))
		{
			SkipOverChar(source, pos, '[');			// Dealt to Hero [
			WriteToChar(source, playerData[playerPos].card1, pos, ' ');	// Jc
			JumpPos(pos, 1);						//  
			WriteToChar(source, playerData[playerPos].card2, pos, ']');	// Kd
			SkipToNextLine(source, pos);
		} // end while
		{
			int gameState = 0;
			while (gameState < SHOWDOWN)
			{
				string movesBuffer;
				movesBuffer.resize(MAXMOVES);

				cout << endl << INDENT << "" << GAMESTATE[gameState] << ": ";

				int movesCounter = 0;
				while (!CheckString(source, pos, NEXTROUND))
				{
					if (!CheckString(source, pos, UNCALLED))		// If it's not something that we don't handle yet
																	// TODO: When handling bet amounts, take this into account
					{
						if (!CheckString(source, pos, playerData[playerPos].name))
						{
							AddAction(playerPos, SKIP , gameState, movesBuffer, movesCounter); // This player didn't make a move, because he folded earlier or something like that
							continue;
						}
						JumpPos(pos, playerData[playerPos].name.length() + 2);
						if (CheckString(source, pos, FOLDS))
						{
							AddAction(playerPos, FOLD , gameState, movesBuffer, movesCounter);
						}
						else if (CheckString(source, pos, CALLS) ||
								 CheckString(source, pos, CHECKS))
						{
							AddAction(playerPos, CALL , gameState, movesBuffer, movesCounter);
						}
						else if (CheckString(source, pos, RAISES) ||
								 CheckString(source, pos, ADDS) ||
								 CheckString(source, pos, BETS))
						{
							AddAction(playerPos, RAISE, gameState, movesBuffer, movesCounter);
						}
						else if (CheckString(source, pos, COLLECTED))
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
					SkipToNextLine(source, pos);
				} // end while
				gameState++;
				SkipToNextLine(source, pos);
				playerPos = 0;
			} // end while
		} // end block
	} // end playerPos
	cout << endl;

	last = SkipToNextGame(source, pos);
}

GameData::GameData (string& source, int& pos)
{
	ParseAll(source, pos);
}