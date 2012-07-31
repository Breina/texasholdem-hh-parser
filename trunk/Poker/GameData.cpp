// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <string>
#include <iostream>
#include "GameData.h"

using namespace std;

const bool DEBUG =		false;
const string DBGGAME =	"0";

const string INDENT =	" -";
const string SPACE =	"   ";

const string FOLDS =	"folds";
const string RAISES =	"raises";
const string CALLS =	"calls";
const string CHECKS =	"checks";
const string BETS =		"bets";
const string DEALT =	"Dealt to";
const string HAS =		"has";
const string NEXTROUND=	"*** ";
const string UNCALLED =	"Uncalled bet";
const string COLLECTED=	"ollected ";
const string SEAT =		"Seat ";
const string INCHIPS =	" in chips";
const string SAID =		"aid"; // Only action where the player name isn't followed by ':', so the SkipString will jump one character too far, cutting off the 's'
const string SB =		"posts small blind";
const string BB =		"posts big blind";
const string ANTE =		"posts the ante";
const string SHOWS =	"shows";

const int SMALLBLIND =	0;
const int BIGBLIND =	1;

const string GAMESTATE[] = {"Preflop", "Flop   ", "Turn   ", "River  "}; // Used cosmeticly

const int PREFLOP =	0;
const int FLOP =	1;
const int TURN =	2;
const int RIVER =	3;
const int SHOWDOWN=	4;
const int END =		5;

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
// Skips the length of the string, plus 2 additional characters (it just happens to be the right amount for the distance between the player name and the action, but also between the action and (if applicable) amount
void GameData::SkipString (string& source, string str, int& pos)
{
	JumpPos(pos, str.length() + 2);
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
// Advances the player position, and makes sure to handle overflow
void GameData::AdvancePlayerPosition(int& playerPos)
{
	playerPos++;
	if (playerPos == pAmount)
		playerPos = 0; // You can write it like p = (p + 1) % pAmount, but it will take more CPU cycles
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
void GameData::SetupPlayerStructs(string players[], string pCash[], int playerPos)
{
	for (int i = 0; i < pAmount; i++)
	{
		playerData[i].name = players[playerPos];
		playerData[i].money = StrToPennies(pCash[playerPos]);
		AdvancePlayerPosition(playerPos);
	} // end for
}
void GameData::CheckNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[])
{
	while (!active[pPos])
	{
		AdvancePlayerPosition(pPos);
		if (DEBUG) cout << " ";
		moveData[movesCounter][gameState].move = SKIP;
		movesCounter++;
	}
}

// Skips players that are inactive, won't increase the player position initially
void GameData::FindNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[])
{
	AdvancePlayerPosition(pPos);
	CheckNextPlayer(pPos, gameState, movesCounter, active);
}
// Adds an action to all players and moves player to next position
void GameData::AddAction(int& pPos, Move action, int gameState, int& movesCounter, bool active[], int betAmount, int pot, int toCall, int stack)
{
	if (movesCounter >= MAXMOVES)
			valid = false;
	if (valid)
	{
		moveData[movesCounter][gameState].move = action;
		moveData[movesCounter][gameState].pot = pot;
		moveData[movesCounter][gameState].betAmount = betAmount;
		moveData[movesCounter][gameState].toCall = toCall;
		moveData[movesCounter][gameState].stack = stack;
		movesCounter++;
	}

	if (action == FOLD)
		active[pPos] = false;

	if (DEBUG) switch (action)
	{
		case FOLD:
			cout << 'f';
			break;
		case CALL:
		case CHECK:
			cout << 'c';
			break;
		case RAISE:
			cout << 'r';
			break;
	}
}
// Returns the amount until character
int GameData::ReadAmount(string& source, int& pos, char c)
{
	string amountStr;
	WriteToChar(source, amountStr, pos, c);
	int betAmount = StrToPennies(amountStr);
	return betAmount;
}

// Main function that parses through the entire source file
void GameData::ParseAll(string& source, int& pos)
{
	SkipOverChar(source, pos, '#');				
	WriteToChar(source, gameID, pos, ':');		
	
	if (DEBUG) cout << "Parsing game #" << gameID << ":";

	if (gameID == DBGGAME)
		system("PAUSE");

	SkipOverChar(source, pos, 'm');
	JumpPos(pos, 1);
	WriteToChar(source, limitType, pos, '(');
	TrimStringEnd(limitType, 1);
	JumpPos(pos, 2);
	sb = ReadAmount(source, pos, '/');
	JumpPos(pos, 2);
	bb = ReadAmount(source, pos, ' ');
	JumpPos(pos, 3);							
	SkipOverChar(source, pos, '\'');
	SkipOverChar(source, pos, '\'');
	JumpPos(pos, 1);
	WriteToChar(source, gameType, pos, ' ');
	SkipOverChar(source, pos, '#');

	int dealer = atoi(source.substr(pos, 1).c_str());
	SkipToNextLine(source, pos);

	{
		string players	[MAXPLAYERS];			// We don't know the amount of players yet, this array is temporarily
		string pCash	[MAXPLAYERS];
		{
			int i = 0;
			while (CheckString(source, pos, SEAT))
			{
				JumpPos(pos, 8);					//
				ParsePlayerName(source, players[i], pos);
				JumpPos(pos, 1);					//
				WriteToChar(source, pCash[i], pos, ' ');
				SkipToNextLine(source, pos);
				i++;
			} // end while
			pAmount = i;									// Player amount
		} // end i
		
		int playerPos = (dealer == pAmount) ? 0 : dealer;
		SetupPlayerStructs(players, pCash, playerPos);			// Orders the players correctly, so 0 = sb, 1 = bb, ..
	} // end players, pCash

	int stacks[MAXPLAYERS];
		for each (int& i in stacks)
			i = 0;
	int pot = 0;
	int playerPos = SMALLBLIND;	// Next expected position, searches around for other names if it doesn't match
	// Handles blinds and antes
	while (!CheckString(source, pos, NEXTROUND))
	{
		for (int i = 0; i < pAmount; i++)
		{
			if (CheckString(source, pos, playerData[playerPos].name))
			{
				SkipString(source, playerData[playerPos].name, pos);

				if (CheckString(source, pos, SAID))
					break;

				SkipOverChar(source, pos, '$');
				int bet = ReadAmount(source, pos, '\n');
				stacks[playerPos] += bet;
				pot += bet;

				AdvancePlayerPosition(playerPos);

				break;
			}
			AdvancePlayerPosition(playerPos);
		}

		SkipToNextLine(source, pos);
	}

	
	/*while (CheckString(source, pos, DEALT))
	{
		SkipOverChar(source, pos, '[');			// Dealt to Hero [
		WriteToChar(source, playerData[playerPos].card1, pos, ' ');	// Jc
		JumpPos(pos, 1);						//  
		WriteToChar(source, playerData[playerPos].card2, pos, ']');	// Kd
		SkipToNextLine(source, pos);
	} // end while*/
	{
		int gameState;

		bool active[MAXPLAYERS];
		for each (bool& b in active)
			b = true;					// Every player is active

		SkipToNextLine(source, pos);
		{
			gameState = 0;
			int movesCounter;
			int toCall = bb;
			while (gameState < SHOWDOWN)
			{
				if (DEBUG) cout << endl << INDENT << "" << GAMESTATE[gameState] << ": ";

				movesCounter = 0;

				if (gameState == 0)
				{
					if (DEBUG) cout << "SB";									// Indicating small blind and big blind for player 0 and 1
					playerPos = 2;									// Skipping the sb and bb, sb and bb is a rule, not a player's strategy, so no point in including them
				}
				else
				{
					playerPos = 0;
					CheckNextPlayer(playerPos, gameState, movesCounter, active);				// Finds the first player, if it's the first round, leaves it to 2
				}

				while (!CheckString(source, pos, NEXTROUND))		// Do this until the next round
				{
					if (CheckString(source, pos, UNCALLED))
					{
						gameState = END;
					}
					else
					{
						if (CheckString(source, pos, playerData[playerPos].name))  // If the correct player's name is found, check what action he did
						{
							SkipString(source, playerData[playerPos].name, pos);
							if (CheckString(source, pos, FOLDS))					// Folds
							{
								AddAction(playerPos, FOLD , gameState, movesCounter, active, 0, 0, toCall, stacks[playerPos]);
								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if (CheckString(source, pos, CALLS))				//Calls
									 
							{
								SkipString(source, CALLS, pos);
								int betAmount = ReadAmount(source, pos, '\n');
								AddAction(playerPos, CALL , gameState, movesCounter, active, betAmount, pot, toCall, stacks[playerPos]);
								stacks[playerPos] += betAmount;
								pot += betAmount;
								FindNextPlayer(playerPos, gameState, movesCounter, active);

								continue;
							}
							else if (CheckString(source, pos, CHECKS))				// Checks
							{
								AddAction(playerPos, CHECK , gameState, movesCounter, active, 0, pot, toCall, stacks[playerPos]);
								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if (CheckString(source, pos, RAISES))				// Raises
							{
								SkipString(source, RAISES, pos);
								int betAmount = ReadAmount(source, pos, ' ');
								betAmount += toCall;	// It says the amount he raised over toCall
								AddAction(playerPos, RAISE , gameState, movesCounter, active, betAmount, pot, toCall, stacks[playerPos]);
								stacks[playerPos] = betAmount;
								toCall = stacks[playerPos];
								pot += betAmount;
								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if	(CheckString(source, pos, BETS))				// Raises
							{
								SkipString(source, BETS, pos);
								int betAmount = ReadAmount(source, pos, '\n');
								AddAction(playerPos, RAISE, gameState, movesCounter, active, betAmount, pot, toCall, stacks[playerPos]);
								stacks[playerPos] += betAmount;
								toCall = stacks[playerPos];
								pot += betAmount;
								FindNextPlayer(playerPos, gameState, movesCounter, active);
								continue;
							}
							else if (CheckString(source, pos, COLLECTED))			// Collected, game ends
							{
								gameState = END;
								break;
							}
							else
							{
								// Ignore action
							}
						}
					}
					SkipToNextLine(source, pos);
				}
				if (gameState < SHOWDOWN)
					gameState++;

				// Positions the position to the first card they need
				switch (gameState)
				{
				case RIVER:
				case TURN:
					SkipOverChar(source, pos, '[');
				case FLOP:									// Could be replaced by default: to speed it up, but it's unsafe
					SkipOverChar(source, pos, '[');
				}

				// Writes the cards to the correct place
				switch (gameState)
				{
				case RIVER:
					WriteToChar(source, communityCards[4], pos, ']');
					break;
				case TURN:
					WriteToChar(source, communityCards[3], pos, ']');
					break;
				case FLOP:									// Could be replaced by default: to speed it up, but it's unsafe
					WriteToChar(source, communityCards[0], pos, ' ');
					JumpPos(pos, 1);
					WriteToChar(source, communityCards[1], pos, ' ');
					JumpPos(pos, 1);
					WriteToChar(source, communityCards[2], pos, ']');
				}

				SkipToNextLine(source, pos);
			} // showdown
		} // end playerPos
				
		if (gameState == SHOWDOWN)
		{
			playerPos = 0;
			while (!active[playerPos])
				playerPos++;

			for (; playerPos < pAmount; playerPos++)
				if (active[playerPos])
				{
					SkipString(source, playerData[playerPos].name, pos);
					if (CheckString(source, pos, SHOWS))
					{
						SkipOverChar(source, pos, '[');
						WriteToChar(source, playerData[playerPos].card1, pos, ' ');
						JumpPos(pos, 1);
						WriteToChar(source, playerData[playerPos].card2, pos, ']');
					}
					SkipToNextLine(source, pos);
				}
		}
	}
	if (DEBUG)				cout << endl;
	if (DEBUG && !valid)	cout << "Game was ignored as it exceeded MAXMOVES: " << MAXMOVES << endl;
	if (DEBUG)				cout << endl;

	last = SkipToNextGame(source, pos);

	//system("PAUSE");
}

GameData::GameData (string& source, int& pos)
{
	valid = true;
	ParseAll(source, pos);
}