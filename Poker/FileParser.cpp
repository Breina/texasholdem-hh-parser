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
	delete DirSpec;
 
    if(hFind == INVALID_HANDLE_VALUE)
    {

		cout << "ERROR: invalid path" << endl;
    }
 
	FindNextFile(hFind, &FindFileData);	// .
	FindNextFile(hFind, &FindFileData); // ..

	for (int i = 0; i < STARTFILE; i++)
		FindNextFile(hFind, &FindFileData);
	int counter = 0;

	string fileName;
	string errorFiles;

    do
    {
		try {
			fileName = FindFileData.cFileName;
			cout << counter + STARTFILE << "/" << TOTALFILES + STARTFILE<< endl << "Reading file: " << fileName << endl;

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
				GameData *gd = new GameData (source, pos);
				if (gd->IsValid())
					dbConnection->StoreGame(*gd);
				end = gd->IsLastGameOfFile();
				delete gd;
			}
		}
		catch (std::exception& e)
		{
			cout << "Error reading " << fileName << ":" << endl;
			cout << e.what();
			errorFiles.append(fileName);
			errorFiles.append("\n");
			system("PAUSE");
		}
		counter++;
    }
	while((FindNextFile(hFind, &FindFileData) != 0)/* && (counter != TOTALFILES)*/);
 
	FindClose(hFind);

	cout << endl << "Parsing finished. Unparsed files:" << errorFiles;
}

FileParser::FileParser (string uri)
{
	dbConnection = new DatabaseWriter (); 
	ParseFiles(uri);
	delete dbConnection;
}