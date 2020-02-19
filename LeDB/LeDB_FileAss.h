#pragma once
#include "LeDB.h"
#include <string>
using namespace std;

class CLeDB_FileAss : public ILeDB_FileAss
{
public:
	CLeDB_FileAss ();
	~CLeDB_FileAss ();

	//ILeDB_FileAss
	//≥ı ºªØ≤Â»Î
	void InitInsert();
	bool InsertAssociation(int id, const char* pchName, int flag);
	bool GetAssociation(int id);
	bool GetAssociation(const char* pchName);
	char* GetExtName(int id);
	void DefaultAss(int flag);
	void SetAssociation(int id, int flag);
	void SetAssociation(const char* pchName, int flag);

	bool CreateTable();
};