#pragma once
#include "LeDB.h"

class CLeDB_Set : public ILeDB_Set
{
public:
	CLeDB_Set();
	~CLeDB_Set();

	// ILeDB_Set	
	void InitSetItem(const char* pchKey, const char* pchValue);
	bool UpdateSingleSet(const char* pchKey, const char* pchValue);
	char* GetSingleSet(const char* pchKey);

	bool CreateTable();
	bool InitSetTable();
};