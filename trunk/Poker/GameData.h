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
	int pot;					// Everything that's on the table
	int betAmount;				// Amount one puts on the table this turn
	int toCall;					// Amount that you need to match
	int stack;					// Amount one has bet in total
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
	void AdvancePlayerPosition(int& playerPos);
	int  StrToPennies(string str);
	void SetupPlayerStructs(string players[], string pCash[], int playerPos);
	void CheckNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[]);
	void FindNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[]);
	void AddAction(int& pPos, Move action, int gameState, int& movesCounter, bool active[], int amount, int pot, int toCall, int stack);
	int  ReadAmount(string& source, int& pos, char c);
	void ParseAll(string& source, int& pos);

public:
	GameData (string& source, int& pos);

	string GetGameID ()	{					return gameID							;}	// Unique gameID
	int	   GetSB ()	{						return sb								;}	// Small blind
	int    GetBB () {						return bb								;}	// Big blind
	string GetLimitType () {				return limitType						;}	// Limit type:  No Limit, ..
	string GetGameType () {					return gameType							;}	// Max players: heads up, 6-max, ..
	int	   GetPlayerAmount () {				return pAmount							;}	// Amount of players in this game

	// playerPos: 0=sb, 1=bb, ...
	string GetPlayerName  (int playerPos) {	return playerData[playerPos].name		;}	// Name of player
	int    GetPlayerCash  (int playerPos) {	return playerData[playerPos].money		;}	// Amount of money at the start of the game
	string GetPlayerCard1 (int playerPos) {	return playerData[playerPos].card1		;}	// First card
	string GetPlayerCard2 (int playerPos) {	return playerData[playerPos].card2		;}	// Second card

	// n: move: 0=sb, 1=bb, ... Loops around | gameState: PREFLOP, FLOP, TURN, RIVER, SHOWDOWN, END
	Move getMove   (int n, int gameState) {	return moveData[n][gameState].move		;}	// Move: SKIP, FOLD, CALL, CHECK, RAISE
	int  getPot    (int n, int gameState) {	return moveData[n][gameState].pot		;}	// Amount of money in the pot
	int  getBet    (int n, int gameState) {	return moveData[n][gameState].betAmount	;}	// Amount of money the player moves to his stack
	int  getToCall (int n, int gameState) {	return moveData[n][gameState].toCall	;}	// Amount of money the player would need to call
	int  getStack  (int n, int gameState) {	return moveData[n][gameState].stack		;}	// Amount of money a player has bet this game

	bool   IsLastGameOfFile () {			return last								;}	// If it's the last game of this file
};

#endif