// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <string>
#include <iostream>
#include "GameData.h"
#include "FileParser.h"

using namespace std;

//const string HHPATH = "F:\\workspace\\Project Poker\\HH\\stars_1979025_1329811231_5-10_NL-Holdem-0-part26-1817512068.txt";
const string HHPATH = "F:\\workspace\\Project Poker\\HH2\\vakenfreak_gmail_com_ONG_NLH4-USD_7-10plrs_x1000k_7826c\\";

int _tmain (int argc, _TCHAR* argv[])
{
	cout << "Program start." << endl;
	FileParser fp (HHPATH);
	cout << "Program finished." << endl;
	//system("PAUSE");
	return EXIT_SUCCESS;
}