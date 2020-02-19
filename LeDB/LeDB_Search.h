#pragma once
#include "LeDB.h"

class CLeDB_Search : public ILeDB_Search
{
public:
	CLeDB_Search();
	~CLeDB_Search();

	// ILeDB_Search
	bool InsertSearch(const char* pchName);
	bool SelectSearch(void** pVoid_String);
	bool DelSearch(const char* pchName);
	bool DelAllSearch();

	bool CreateTable();
};
