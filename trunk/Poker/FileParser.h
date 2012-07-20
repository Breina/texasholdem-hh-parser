#ifndef FILEPARSER_H
#define FILEPARSER_H

using namespace std;

class FileParser
{
private:
	void GetFile (string& source, string path);

public:
	FileParser (string URI, string output);

};

#endif