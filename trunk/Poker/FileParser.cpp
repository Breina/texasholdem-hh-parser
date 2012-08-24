#include "stdafx.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <ctime>

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
		std::cout << "ERROR: invalid path" << endl;
		system("PAUSE");
		exit(1);
     }

	FindNextFile(hFind, &FindFileData);	// .
	FindNextFile(hFind, &FindFileData); // ..

	int totalFiles = 0;
	while (FindNextFile(hFind, &FindFileData) != 0)
		totalFiles++;	
	int totalGames = totalFiles * 1000;

	hFind = FindFirstFile(DirSpec, &FindFileData);
	FindNextFile(hFind, &FindFileData);	// .
	FindNextFile(hFind, &FindFileData); // ..

	delete DirSpec;

	int counter = 0;
	int readTime = 0;
	int parseTime = 0;
	int dbTime = 0;
	int lastDraw = 0;
	int gamesAmount = 0;
	int startTime = clock();

	string fileName;
	string errorFiles;

    do
    {
		try {
			fileName = FindFileData.cFileName;
			//cout << counter << "/" << totalFiles << endl << "Reading file: " << fileName << endl;

			//fileName = DBGFILE;
			//if (fileName == DBGFILE)
			//	cout << "";

			string source;
			int t = clock();
			GetFile(source, uri + fileName);
			readTime += clock() - t;

			//cout << "Fixing file..." << endl;
			//FixFile(source);

			//cout << "Parsing file..." << endl;
			int pos = 0;
			bool end = false;
			while (!end)
			{
				int t = clock();
				GameData *gd = new GameData (source, pos);
				parseTime += clock() - t;

				if (gd->IsValid())
				{
					int t = clock();
					dbConnection->StoreGame(*gd);
					dbTime += clock() - t;
				}

				gamesAmount++;

				if (clock() - lastDraw > 50)
				{
					int totalTime = readTime + parseTime + dbTime;
					int gps = gamesAmount / ((clock() - startTime) / 1000.0);
					printf("File: %i/%i GPS: %i R(%i%%) P(%i%%) DB(%i%%) Est.: %im Game: %s ", counter, totalFiles, gps, 
						100 * readTime  / totalTime, 100 * parseTime / totalTime, 100 * dbTime / totalTime,
						(totalGames - gamesAmount) / gps / 60, gd->GetGameID().c_str());
					cout << "\r" << flush;					
					
					lastDraw = clock();
				}

				end = gd->IsLastGameOfFile();
				delete gd;
			}
		}
		catch (std::exception& e)
		{
			std::cout << "Error reading " << fileName << ":" << endl;
			std::cout << e.what();
			errorFiles.append(fileName);
			errorFiles.append("\n");
			system("PAUSE");
		}
		counter++;
    }
	while(FindNextFile(hFind, &FindFileData) != 0);
 
	FindClose(hFind);

	std::cout << endl << "Parsing finished. Unparsed files:" << errorFiles;
}

FileParser::FileParser (string uri, bool truncate)
{
	dbConnection = new DatabaseWriter ();
	if (truncate)
	{
		std::cout << "Truncating database..." << endl;
		dbConnection->InitDB();
	}
	std::cout << "Parsing files..." << endl;
	ParseFiles(uri);
	delete dbConnection;
}