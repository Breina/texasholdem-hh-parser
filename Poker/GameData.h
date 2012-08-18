#ifndef GAMEDATA_H
#define GAMEDATA_H

using namespace std;

const int MAXPLAYERS = 9;
const int MAXMOVES = 40;

const bool DEBUG =		false;
const string DBGGAME =	"";

const string INDENT =	" -";
const string SPACE =	"   ";

const string FOLDS =	"folds";
const string RAISES =	"raises";
const string CALLS =	"calls";
const string CHECKS =	"checks";
const string BETS =		"bets";
const string DEALT =	"Dealt to";
const string DEALING =	"Dealing";
const string HAS =		"has";
const string NEXT =		"---\n";
const string NEXTROUND=	"--- ";
//const string UNCALLED =	"Uncalled bet";
//const string COLLECTED=	"ollected ";
const string SEAT =		"Seat ";
//const string SAID =		"aid"; // Only action where the player name isn't followed by ':', so the SkipString will jump one character too far, cutting off the 's'
const string SB =		"posts small blind";
const string BB =		"posts big blind";
const string ANTE =		"posts the ante";
//const string SHOWS =	"shows";

const int SMALLBLIND =	0;
const int BIGBLIND =	1;

const string GAMESTATE[] = {"Preflop", "Flop   ", "Turn   ", "River  "}; // Used cosmeticly

const int PREFLOP =	0;
const int FLOP =	1;
const int TURN =	2;
const int RIVER =	3;
const int SHOWDOWN=	4;
const int END =		5;

enum Move {SKIP, FOLD, CALL, CHECK, RAISE};

struct PlayerData
{
	string name;
	string card1;				// TODO: Initilize with size 2
	string card2;				// TODO: Initilize with size 2
};

struct MoveData
{
	Move move;
	int pot;	// ALL VALUES ARE CENTS
	int bet;
	int toCall;
	int currentBet;
	int stack;
};

class GameData
{
private:
	string gameID;				// The game ID
	string gameType;			// 6 max, heads up, ..
	int sb, bb;					// Small blind, big blind
	string limitType;			// No limit, fixed limit
	int pAmount;				// Amount of players
	int movesLengths[4];		// Lengths of the betting rounds
	int roundsPlayed;			// Amount of rounds played
	PlayerData playerData[MAXPLAYERS];
	MoveData moveData[MAXMOVES][4];

	string communityCards[5];	// TODO: Initilize with size 2

	bool last;					// If there are more games after this one
	bool valid;					// If it didn't exceed MAXMOVES

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
	int  StrToInt(string str);
	int  StrToPennies(string str);
	void SetupPlayerStructs(string players[], int playerPos);
	void CheckNextPlayer(int& pPos, int gameState, int& movesCounter, bool active[]);
	void FindNextPlayer(int& pPos, int& gameState, int& movesCounter, bool active[]);
	void AddAction(int& pPos, Move action, int gameState, int& movesCounter, bool active[], int amount, int pot, int toCall, int currentBet, int stack);
	string ReadAmount(string& source, int& pos, char c);
	string ReadAmount2(string& source, int& pos, char c1, char c2);
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
	PlayerData GetPlayer  (int playerPos) { return playerData[playerPos]			;}	// Player
	string GetPlayerName  (int playerPos) {	return playerData[playerPos].name		;}	// Name of player
	string GetPlayerCard1 (int playerPos) {	return playerData[playerPos].card1		;}	// First card
	string GetPlayerCard2 (int playerPos) {	return playerData[playerPos].card2		;}	// Second card

	int  GetRoundsPlayed ()				  { return roundsPlayed						;}	// Amount of betting rounds played
	int* GetMovesLengths ()				  { return movesLengths						;}	// Array of the lower four
	int  GetPreflopMovesLength ()		  { return movesLengths[PREFLOP]			;}	// Length of preflop moves
	int  GetFlopMovesLength ()			  { return movesLengths[FLOP]				;}	// Length of flop moves
	int  GetTurnMovesLength ()			  { return movesLengths[TURN]				;}	// Length of turn moves
	int  GetRiverMovesLength ()			  { return movesLengths[RIVER]				;}	// Length of river moves

	// n: move: 0=sb, 1=bb, ... Loops around | gameState: PREFLOP, FLOP, TURN, RIVER, SHOWDOWN, END
	MoveData GetMove(int n, int gameState){ return moveData[n][gameState]			;}	// Returns the MoveData				TODO: REMOVE
	Move GetAction (int n, int gameState) {	return moveData[n][gameState].move		;}	// Move: SKIP, FOLD, CALL, CHECK, RAISE
	int  GetPot    (int n, int gameState) {	return moveData[n][gameState].pot		;}	// Amount of money in the pot
	int  GetBet    (int n, int gameState) {	return moveData[n][gameState].bet		;}	// Amount of money the player moves outward
	int  GetToCall (int n, int gameState) {	return moveData[n][gameState].toCall	;}	// Amount of money the player would need to call
	int  GetCurBet (int n, int gameState) { return moveData[n][gameState].currentBet;}	// Amount of money a player has bet this game
	int  GetStack  (int n, int gameState) {	return moveData[n][gameState].stack		;}	// Amount of money a player has this game

	bool IsLastGameOfFile () {				return last								;}	// If it's the last game of this file
	bool IsValid() {						return valid							;}	// If it doesn't overflow MAXMOVES
};

#endif