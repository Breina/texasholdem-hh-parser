#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "DatabaseWriter.h"
#include "GameData.h"

using namespace std;

const string DBGFILE =	"ong ONG hand-histories.com_165.txt";
//const int STARTFILE = 0;
const int TOTALFILES =	1019;

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