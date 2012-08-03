#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "DatabaseWriter.h"
#include "GameData.h"

using namespace std;

const string DBGFILE =	"";
const int TOTALFILES =	1018;

class FileParser
{
private:
	string folderPath;
	string files[];

	void GetFile (string& source, string path);
	void FixFile (string& source);
	void ParseFiles (string uri);

public:
	FileParser (string uri);

};

#endif