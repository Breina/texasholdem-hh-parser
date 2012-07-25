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

void GetFileList (string uri)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH]; // directory specification
 
	cout<<"Path: ";
	cin.get(DirSpec, MAX_PATH);
	cout<<"\n";
	strncat(DirSpec, "\\*", 3);
	hFind = FindFirstFile(DirSpec, &FindFileData);
 
    if(hFind == INVALID_HANDLE_VALUE)
    {
		cout<<"Error: invalid path\n";
    }
 
	cout<<FindFileData.cFileName<<"\n";
 
    while(FindNextFile(hFind, &FindFileData) != 0)
    {
		cout<<FindFileData.cFileName<<"\n";
    }
 
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