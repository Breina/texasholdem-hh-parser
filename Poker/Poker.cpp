// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <string>
#include <iostream>
#include "GameData.h"
#include "FileParser.h"

using namespace std;

//const string HHPATH = "F:\\workspace\\Project Poker\\HH\\stars_1979025_1329811231_5-10_NL-Holdem-0-part26-1817512068.txt";
const string HHPATH = "F:\\workspace\\Project Poker\\HH\\";

int _tmain (int argc, _TCHAR* argv[])
{
	FileParser fp (HHPATH);

	system("PAUSE");
	return EXIT_SUCCESS;
}