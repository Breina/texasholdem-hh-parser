// Poker.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <string>
#include <iostream>
#include "GameData.h"

using namespace std;

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
	bool found = false;
	while (!found)
	{
		WriteToChar(source, dest, pos, '$');
		for (int p = ++pos; source[p] != '$'; p++)
			if (source[p] == '\n')	// If it meets a newline sooner than another $, we know the $ was for the money, not the player name
			{
				found = true;
				break;
			}
		if (!found)
		{
			dest.append("$");
			pos++;
		}
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
	JumpPos(pos, str.length() + 1);
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
// Converts a string to int, so we won't have to deal with floats
int GameData::StrToInt(string str)
{
	int ret = atoi(str.c_str());
	return ret;
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
void GameData::SetupPlayerStructs(string players[], int playerPos)
{
	for (int i = 0; i < pAmount; i++)
	{
		playerData[i].name = players[playerPos];
		AdvancePlayerPosition(playerPos);
	} // end for
}
// Forgot what this one does >.<
void GameData::CheckNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[])
{
	while (!active[pPos])
	{
		AdvancePlayerPosition(pPos);
		if (DEBUG) cout << " ";
		if (valid) moveData[movesCounter][gameState].move = SKIP;
		movesCounter++;
	}
}
// Skips players that are inactive, won't increase the player position initially
void GameData::FindNextPlayer(int& pPos, int& gameState, int& movesCounter, bool active[])
{
	AdvancePlayerPosition(pPos);
	int i = 0;
	bool b;
	for (int j = 0; j < pAmount; j++)
		if (b = active[j])
		{
			i++;
			if (i == 2)
				break;
		}
	if (i == 1) gameState = END;
	CheckNextPlayer(pPos, gameState, movesCounter, active);
}
// Adds an action to all players and moves player to next position
void GameData::AddAction(int& pPos, Move action, int gameState, int& movesCounter, bool active[], int betAmount, int pot, int toCall, int currentBet, int stack)
{
	if (movesCounter >= MAXMOVES)
	{
		cout << endl << "Game was ignored as it exceeded MAXMOVES: " << MAXMOVES << endl;
		return;
	}
	moveData[movesCounter][gameState].move = action;
	moveData[movesCounter][gameState].pot = pot;
	moveData[movesCounter][gameState].betAmount = betAmount;
	moveData[movesCounter][gameState].toCall = toCall;
	moveData[movesCounter][gameState].currentBet = currentBet;
	movesCounter++;

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
string GameData::ReadAmount(string& source, int& pos, char c)
{
	string amountStr;
	WriteToChar(source, amountStr, pos, c);
	return amountStr;
}
// Like ReadAmount, but with 2 possible characters
string GameData::ReadAmount2(string& source, int& pos, char c1, char c2)
{
	string amountStr;
	string s;
	while (source[pos] != c1 && source[pos] != c2)
	{
		s = source[pos];
		amountStr.append(s);
		pos++;
	}
	return amountStr;
}

// Main function that parses through the entire source file
void GameData::ParseAll(string& source, int& pos)
{
	SkipToChar(source, pos, 'R');				
	WriteToChar(source, gameID, pos, ' ');		
	
	if (DEBUG) cout << endl << "Parsing game " << gameID << ":";

	if (gameID == DBGGAME)
		system("PAUSE");

	SkipOverChar(source, pos, '(');
	WriteToChar(source, limitType, pos, ' ');
	JumpPos(pos, 1);
	WriteToChar(source, gameType, pos, ' ');
	JumpPos(pos, 2);
	sb = StrToPennies(ReadAmount(source, pos, '/'));
	JumpPos(pos, 2);
	bb = StrToPennies(ReadAmount(source, pos, ','));
	SkipToNextLine(source, pos);
	SkipToNextLine(source, pos);
	JumpPos(pos, 13);
	int playerPos = StrToInt(ReadAmount(source, pos, '\n'));
	JumpPos(pos, 18);
	pAmount = StrToInt(ReadAmount(source, pos, '\n'));
	int first;	// Who is first in the absolute order from the relative order
	SkipToNextLine(source, pos);

	int stacks [MAXPLAYERS];
	{
		string players	[MAXPLAYERS];
		int    pCash	[MAXPLAYERS];
		{
			int i = 0;
			bool foundButton = false;
			while (CheckString(source, pos, SEAT))
			{
				JumpPos(pos, 5);
				if (playerPos == StrToInt(ReadAmount(source, pos, ':')))
				{
					playerPos = i;
					foundButton = true;
				}
				JumpPos(pos, 2);
				ParsePlayerName(source, players[i], pos);
				string s;
				WriteToChar(source, s, pos, ')');
				pCash[i] = StrToPennies(s);
				SkipToNextLine(source, pos);
				i++;
			} // end while
			if (!foundButton)
			{
				if (DEBUG)
					cout << endl << "Button player not found, skipping game..." << endl;
				return;
			}
			if (i != pAmount)
			{
				if (DEBUG)
					cout << endl << "'Players in round' appears wrong. Assuming correct amount is " << i << ".";
				pAmount = i;
			}
			first = pAmount - playerPos - 1;
		} // end i
		
		AdvancePlayerPosition(playerPos);

		int j = playerPos;
		for (int i = 0; i < MAXPLAYERS; i++)
		{
			stacks[i] = pCash[j];
			AdvancePlayerPosition(j);
		}

		SetupPlayerStructs(players, playerPos);			// Orders the players correctly, so 0 = sb, 1 = bb, ..
	} // end players, stacks

	int currentBets[MAXPLAYERS];
	for each (int& i in currentBets)
		i = 0;
	int pot = 0;
	playerPos = SMALLBLIND;	// Next expected position, searches around for other names if it doesn't match
	// Handles blinds and antes
	while (!CheckString(source, pos, NEXT))
	{
		bool found = false;;
		for (int i = 0; i < pAmount; i++)
		{
			if (CheckString(source, pos, playerData[playerPos].name))
			{
				SkipString(source, playerData[playerPos].name, pos);

				//if (CheckString(source, pos, SAID))
				//	break;

				SkipOverChar(source, pos, '$');
				int bet = StrToPennies(ReadAmount(source, pos, '\n'));
				currentBets[playerPos] += bet;
				stacks[playerPos] -= bet;
				pot += bet;
				found = true;

				JumpPos(pos, 1);
				AdvancePlayerPosition(playerPos);
				break;
			}
			/*else
			{
				counter++;
			}*/
			AdvancePlayerPosition(playerPos);
		}
		if (!found)
		{
			if (DEBUG)
				cout << endl << "Unknown player name. Skipping game..." << endl;
			return;
		}
	}
	do
	{
		SkipToNextLine(source, pos);
	}
	while (!CheckString(source, pos, DEALING));

	{
		int gameState;

		bool active[MAXPLAYERS];
		for each (bool& b in active)
			b = true;					// Every player is active

		SkipToNextLine(source, pos);
		{
			gameState = PREFLOP;
			int movesCounter;
			//int toCall = bb;
			int toCall = 0;
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

				while (!CheckString(source, pos, NEXTROUND) || !CheckString(source, pos, NEXT))		// Do this until the next round
				{
					//if (CheckString(source, pos, UNCALLED))
					//{
					//	gameState = END;
					//}
					//else
					{
						if (CheckString(source, pos, playerData[playerPos].name))  // If the correct player's name is found, check what action he did
						{
							SkipString(source, playerData[playerPos].name, pos);
							if (CheckString(source, pos, FOLDS))					// Folds
							{
								AddAction(playerPos, FOLD , gameState, movesCounter, active, 0, pot, toCall, currentBets[playerPos], stacks[playerPos]);
								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if (CheckString(source, pos, CALLS))				//Calls 
							{
								SkipString(source, CALLS, pos);
								JumpPos(pos, 1);
								int betAmount = StrToPennies(ReadAmount2(source, pos, '\n', ' ')) - currentBets[playerPos];

								AddAction(playerPos, CALL , gameState, movesCounter, active, betAmount, pot, toCall, currentBets[playerPos], stacks[playerPos]);

								currentBets[playerPos] += betAmount;
								stacks[playerPos] -= betAmount;
								pot += betAmount;

								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if (CheckString(source, pos, CHECKS))				// Checks
							{
								AddAction(playerPos, CHECK , gameState, movesCounter, active, 0, pot, toCall, currentBets[playerPos], stacks[playerPos]);
								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if (CheckString(source, pos, RAISES))				// Raises
							{
								SkipString(source, RAISES, pos);
								JumpPos(pos, 1);

								int betAmount = (StrToPennies(ReadAmount(source, pos, ' ')) + toCall) - currentBets[playerPos];

								AddAction(playerPos, RAISE , gameState, movesCounter, active, betAmount, pot, toCall, currentBets[playerPos], stacks[playerPos]);
								
								currentBets[playerPos] += betAmount;
								stacks[playerPos] -= betAmount;
								toCall = currentBets[playerPos];
								pot += betAmount;
								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							else if	(CheckString(source, pos, BETS))				// Raises
							{
								SkipString(source, BETS, pos);
								JumpPos(pos, 1);

								int betAmount = StrToPennies(ReadAmount2(source, pos, '\n', ' ')) - currentBets[playerPos];

								AddAction(playerPos, RAISE, gameState, movesCounter, active, betAmount, pot, toCall, currentBets[playerPos], stacks[playerPos]);

								currentBets[playerPos] += betAmount;
								stacks[playerPos] -= betAmount;
								toCall = currentBets[playerPos];
								pot += betAmount;

								FindNextPlayer(playerPos, gameState, movesCounter, active);
							}
							//else if (CheckString(source, pos, COLLECTED))			// Collected, game ends
							//{
							//	gameState = END;
							//	break;
							//}
							else
							{
								// Ignore action
							}
						}
						else
						{
							if (DEBUG)
								cout << endl << "Didn't find expected player, skipping game..." << endl;
							return;
						}
					}
					SkipToNextLine(source, pos);
				}
				if (gameState < SHOWDOWN)
					gameState++;
				if (gameState < SHOWDOWN)
				{
					SkipOverChar(source, pos, '[');
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
				}
				SkipToNextLine(source, pos);
			}
		} // end playerPos
				
		SkipToNextLine(source, pos);
		SkipToNextLine(source, pos);
		
		playerPos = first;
		for (int i = 0; i < pAmount; i++)
		{
			JumpPos(pos, 8);
			if (CheckString(source, pos, playerData[playerPos].name))
			{
				SkipString(source, playerData[playerPos].name, pos);
				for (; source[pos] != '\n'; pos++)
					if (source[pos] == '[')
					{
						JumpPos(pos, 1);
						WriteToChar(source, playerData[playerPos].card1, pos, ',');
						JumpPos(pos, 2);
						WriteToChar(source, playerData[playerPos].card2, pos, ']');
					}
				JumpPos(pos, 1);	
			}
			AdvancePlayerPosition(playerPos);
		}
			
	}

	if (DEBUG)
		cout << endl;

	valid = true;
	//system("PAUSE");
}

GameData::GameData (string& source, int& pos)
{
	valid = true;
	ParseAll(source, pos);
	last = SkipToNextGame(source, pos);
	if (DEBUG)
		cout << endl;
}