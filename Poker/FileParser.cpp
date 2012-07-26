#include "stdafx.h"
#include <string>
#include <iostream>
#include <windows.h>
#include <fstream>
#include "FileParser.h"
#include "GameData.h"

using namespace std;



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
		cout<< "Reading file: " << fileName << endl;
		string source;
		GetFile(source, uri + fileName);
		int pos = 0;
		bool end = false;
		while (!end)
		{
			GameData gd (source, pos);
			end = gd.IsLastGameOfFile();
			//delete gd;
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