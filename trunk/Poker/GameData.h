#ifndef GAMEDATA_H
#define GAMEDATA_H

using namespace std;

const int MAXPLAYERS = 10;
const int MAXMOVES = 500;

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

	void skipToChar (string& source, int& pos, char c);
	void skipOverChar (string& source, int& pos, char c);
	void writeToChar (string& source, string& dest, int& pos, char c);
	void GameData::parsePlayerName (string& source, string& dest, int& pos);
	void skipToNextLine(string& source, int& pos);
	bool GameData::skipToNextGame(string& source, int& pos);
	void jumpPos (int& pos, int amount);
	void trimStringEnd (string& s, int amount);
	bool checkString(string& source, int& pos, string match);
	int readPlayerName(string& source, int& pos, string players[]);
	void advancePlayerPosition(int& p);
	void setPlayerPositions(string players[], int& p, int pPosition[]);
	int strToPennies(string str);
	void setupPlayerStructs(string players[], string pCash[], int pPosition[]);
	void addAction(int& pPos, char action, int gameState, string& movesBuffer, int& movesCounter);
	void parseAll(string& source, int& pos);

public:
	GameData (string& source, int& pos);

	string getGameID ()	{					return gameID						;}
	string getSB ()	{						return sb							;}
	string getBB () {						return bb							;}
	string getLimitType () {				return limitType					;}
	int	   getPlayerAmount () {				return pAmount						;}
	string getPlayerName (int playerPos) {	return playerData[playerPos].name	;}
	int    getPlayerCash (int playerPos) {	return playerData[playerPos].money	;}
	string getPlayerCard1 (int playerPos) {	return playerData[playerPos].card1	;}
	string getPlayerCard2 (int playerPos) {	return playerData[playerPos].card2	;}
	
	string getPlayerMoves(int playerPos, int gameState) {
		return playerData[playerPos].moves[gameState];
	}

	bool isLastGameOfFile () {				return last							;}
};

#endif