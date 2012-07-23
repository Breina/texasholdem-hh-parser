#ifndef GAMEDATA_H
#define GAMEDATA_H

using namespace std;

const int MAXPLAYERS = 6;
const int MAXMOVES = 24;

enum Move {SKIP, FOLD, CALL, CHECK, RAISE};

struct PlayerData
{
	string name;
	string card1;				// TODO: Initilize with size 2
	string card2;				// TODO: Initilize with size 2
	int money;
};

struct MoveData
{
	Move move;
	int pot;
	// Add more stats
};

class GameData
{
private:
	string gameID;				// The game ID
	string gameType;			// 6 max, heads up, ..
	int sb, bb;					// Small blind, big blind
	string limitType;			// No limit, fixed limit
	int pAmount;				// Amount of players
	PlayerData playerData[MAXPLAYERS];
	MoveData moveData[MAXMOVES][4];

	string communityCards[5];	// TODO: Initilize with size 2

	bool last;					// If there are more games after this one

	void SkipToChar (string& source, int& pos, char c);
	void SkipOverChar (string& source, int& pos, char c);
	void WriteToChar (string& source, string& dest, int& pos, char c);
	void ParsePlayerName (string& source, string& dest, int& pos);
	void SkipToNextLine(string& source, int& pos);
	bool SkipToNextGame(string& source, int& pos);
	void JumpPos (int& pos, int amount);
	void SkipString (string& source, string str, int& pos);
	void TrimStringEnd (string& s, int amount);
	bool CheckString(string& source, int& pos, string match);
	int  ReadPlayerName(string& source, int& pos, string players[]);
	void AdvancePlayerPosition(int& playerPos);
	int  StrToPennies(string str);
	void SetupPlayerStructs(string players[], string pCash[], int& playerPos);
	void FindNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[]);
	void AddAction(int& pPos, Move action, int gameState, int& movesCounter, bool active[], int amount, int pot);
	int  ReadAmount(string& source, int& pos, char c);
	void ParseAll(string& source, int& pos);

public:
	GameData (string& source, int& pos);

	string GetGameID ()	{					return gameID						;}
	int	   GetSB ()	{						return sb							;}
	int    GetBB () {						return bb							;}
	string GetLimitType () {				return limitType					;}
	int	   GetPlayerAmount () {				return pAmount						;}
	string GetPlayerName (int playerPos) {	return playerData[playerPos].name	;}
	int    GetPlayerCash (int playerPos) {	return playerData[playerPos].money	;}
	string GetPlayerCard1 (int playerPos) {	return playerData[playerPos].card1	;}
	string GetPlayerCard2 (int playerPos) {	return playerData[playerPos].card2	;}

	bool   IsLastGameOfFile () {			return last						;}
};

#endif