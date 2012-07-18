#ifndef GAMEDATA_H
#define GAMEDATA_H

using namespace std;

const int MAXPLAYERS = 10;
const int MAXMOVES = 12;

struct PlayerData {
	string name;
	string card1, card2;
	int money;		// At start of the game
	string moves[4];
};

class GameData
{
private:
	string gameID;				// The game ID
	string gameType;			// 6 max, heads up, ..
	string sb, bb;				// Small blind, big blind
	string limitType;			// No limit, fixed limit
	int pAmount;				// Aantal spelers
	PlayerData playerData[MAXPLAYERS];	// Player structs

	bool last;					// If there are more games after this one

	void SkipToChar (string& source, int& pos, char c);
	void SkipOverChar (string& source, int& pos, char c);
	void WriteToChar (string& source, string& dest, int& pos, char c);
	void ParsePlayerName (string& source, string& dest, int& pos);
	void SkipToNextLine(string& source, int& pos);
	bool SkipToNextGame(string& source, int& pos);
	void JumpPos (int& pos, int amount);
	void TrimStringEnd (string& s, int amount);
	bool CheckString(string& source, int& pos, string match);
	int  ReadPlayerName(string& source, int& pos, string players[]);
	void AdvancePlayerPosition(int& p);
	void SetPlayerPositions(string players[], int& p, int pPosition[]);
	int  StrToPennies(string str);
	void SetupPlayerStructs(string players[], string pCash[], int pPosition[]);
	void AddAction(int& pPos, char action, int gameState, string& movesBuffer, int& movesCounter);
	void ParseAll(string& source, int& pos);

public:
	GameData (string& source, int& pos);

	string GetGameID ()	{					return gameID						;}
	string GetSB ()	{						return sb							;}
	string GetBB () {						return bb							;}
	string GetLimitType () {				return limitType					;}
	int	   GetPlayerAmount () {				return pAmount						;}
	string GetPlayerName (int playerPos) {	return playerData[playerPos].name	;}
	int    GetPlayerCash (int playerPos) {	return playerData[playerPos].money	;}
	string GetPlayerCard1 (int playerPos) {	return playerData[playerPos].card1	;}
	string GetPlayerCard2 (int playerPos) {	return playerData[playerPos].card2	;}
	
	string GetPlayerMoves(int playerPos, int gameState) {
		return playerData[playerPos].moves[gameState];
	}

	bool   IsLastGameOfFile () {			return last						;}
};

#endif