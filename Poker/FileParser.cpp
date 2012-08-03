#include "stdafx.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <fstream>
#include "FileParser.h"
#include "DatabaseWriter.h"
#include "GameData.h"

using namespace std;

DatabaseWriter* dbConnection;

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

// Fixes the hh-writer's errors
// î»¿
// â‚¬
void FileParser::FixFile (string& source)
{
	int j = 0;
	int i;
	for (i = 0; i < source.length(); i++)
	{
		switch (source[i])
		{
		case 'î':
			if (source[i + 1] == '»' &&
				source[i + 2] == '¿')
			{
				i += 3;
				source[j] = '$';
			}
			else
			{
				source[j] = source[i];
			}
			break;
		case 'â':
			if (source[i + 1] == '‚' &&
				source[i + 2] == '¬')
			{
				i += 3;
				source[j] = '$';
			}
			else
			{
				source[j] = source[i];
			}
			break;
		case '€':
			source[j] = '$';
			break;
		default:
			source[j] = source[i];
		}
		j++;
	}
	source.resize(j);
}

void FileParser::ParseFiles (string uri)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char * DirSpec = new char [MAX_PATH]; // directory specification
	strcpy(DirSpec, uri.c_str());

	strncat(DirSpec, "*", 3);
	hFind = FindFirstFile(DirSpec, &FindFileData);
 
    if(hFind == INVALID_HANDLE_VALUE)
    {
		cout<<"ERROR: invalid path" << endl;
    }
 
	FindNextFile(hFind, &FindFileData);	// .
	FindNextFile(hFind, &FindFileData); // ..
	string fileName;
	int i = 0;

    do
    {
		fileName = FindFileData.cFileName;
		cout << i << "/" << TOTALFILES << endl << "Reading file: " << fileName << endl;

		//fileName = DBGFILE;
		//if (fileName == DBGFILE)
		//	cout << "";

		string source;
		GetFile(source, uri + fileName);

		//cout << "Fixing file..." << endl;
		//FixFile(source);

		cout << "Parsing file..." << endl;
		int pos = 0;
		bool end = false;
		while (!end)
		{
			GameData gd (source, pos);
			dbConnection->StoreGame(gd);
			end = gd.IsLastGameOfFile();
		}
		i++;
    }
	while(FindNextFile(hFind, &FindFileData) != 0);
 
	FindClose(hFind);
}

FileParser::FileParser (string uri)
{
	dbConnection = new DatabaseWriter (); 
	ParseFiles(uri);


}