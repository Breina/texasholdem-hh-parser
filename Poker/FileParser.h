#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "GameData.h"

using namespace std;

class FileParser
{
private:
	void GetFile (string& source, string path);
	void GetFileList (string uri);
	void WriteToDb (GameData& gd);

public:
	FileParser (string uri, string output);

};

#endif