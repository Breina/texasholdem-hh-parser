// Poker.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include "GameData.h"

using namespace std;

const string HHPATH = "F:\\workspace\\Project Poker\\HH\\stars_1979025_1329811231_5-10_NL-Holdem-0-part26-1817512068.txt";

void getFile (string& source, string path)
	{
		ifstream inFile;
		string s;
		inFile.open ( path );
		while(!inFile.eof()) // To get you all the lines.
		{
			getline(inFile, s); // Saves the line in STRING.
			source.append(s);
			source.append("\n");
		}
		inFile.close();
}

int _tmain (int argc, _TCHAR* argv[])
{
	string source;
	getFile(source, HHPATH);
	int pos = 0;

	GameData gd1 (source, pos);
	GameData gd2 (source, pos);
	GameData gd3 (source, pos);
	GameData gd4 (source, pos);
	GameData gd5 (source, pos);
	GameData gd6 (source, pos);
	GameData gd7 (source, pos);
	GameData gd8 (source, pos);
	GameData gd9 (source, pos);

	system("PAUSE");
	return EXIT_SUCCESS;
}