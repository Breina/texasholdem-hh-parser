#include "stdafx.h"
#include <string>
#include <iostream>
#include <fstream>
#include "FileParser.h"
//#include "GameData.h"

//using namespace std;

void FileParser::GetFile (string& source, string path)
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

void FileParser::Init (string& URI, string& output)
{
	string source;
	FileParser::GetFile(source, URI);
	int pos = 0;
	
	bool end = false;
	while (!end)
	{
		//GameData gd (source, pos);

		end = gd.IsLastGameOfFile();
	}
}