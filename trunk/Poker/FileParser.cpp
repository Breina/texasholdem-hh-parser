#include "stdafx.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <fstream>
#include "FileParser.h"
#include "GameData.h"

using namespace std;

const string DBGFILE =	"stars_1979025_1329862332_5-10_NL-Holdem-0-part26-802429293.txt";

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

void FileParser::GetFileList (string uri)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char * DirSpec = new char [MAX_PATH]; // directory specification
	strcpy(DirSpec, uri.c_str());

	strncat(DirSpec, "*", 3);
	hFind = FindFirstFile(DirSpec, &FindFileData);
 
    if(hFind == INVALID_HANDLE_VALUE)
    {
		cout<<"Error: invalid path\n";
    }
 
	FindNextFile(hFind, &FindFileData);	// .
	FindNextFile(hFind, &FindFileData); // ..
	string fileName;
    do
    {
		fileName = FindFileData.cFileName;
		cout << "Reading file: " << fileName << endl;

		fileName = DBGFILE;
		//if (fileName == DBGFILE)
		//	cout << "";

		string source;
		GetFile(source, uri + fileName);

		cout << "Fixing file..." << endl;
		FixFile(source);

		cout << "Parsing file..." << endl;
		int pos = 0;
		bool end = false;
		while (!end)
		{
			GameData gd (source, pos);
			end = gd.IsLastGameOfFile();
		}
    }
	while(FindNextFile(hFind, &FindFileData) != 0);
 
	FindClose(hFind);
}

void FileParser::WriteToDb (GameData& gd)
{

}

FileParser::FileParser (string uri, string output)
{
	GetFileList(uri);

	//string source;
	//GetFile(source, URI);
	//int pos = 0;
	
	//bool end = false;
	//while (!end)
	//{
		//GameData gd (source, pos);

		//end = gd.IsLastGameOfFile();
	//}
}