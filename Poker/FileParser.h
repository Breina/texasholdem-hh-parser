#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "GameData.h"

using namespace std;

class FileParser
{
private:
	string folderPath;
	string files[];

	void GetFile (string& source, string path);
	void FixFile (string& source);
	void ParseFiles (string uri);
	void WriteToDb (GameData& gd);

public:
	FileParser (string uri);

};

#endif