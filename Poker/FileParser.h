#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "stdafx.h"

using namespace std;

class FileParser
{
private:
	void GetFile (string& source, string& path);

public:
	void Init (string& URI, string& output);


#endif