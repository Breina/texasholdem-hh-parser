#ifndef FILEPARSER_H
#define FILEPARSER_H

#include "DatabaseWriter.h"

using namespace std;

class DatabaseWriter
{
private:

public:
	void CloseConn(PGconn *conn);
	PGconn *ConnectDB ();

	DatabaseWriter ();

};

#endif