#pragma once
#include "LeDB.h"

class CLeDB_HotKey : public ILeDB_HotKey
{
public:
	CLeDB_HotKey();
	~CLeDB_HotKey();

	// ILeDB_HotKey	
	void DefaultHotkey();
	void InsertKotKey(const char* pchName, int key1, int key2);
	void GetHotkey(const char* pchName, _HotKey& hk);
	void SetHotkey(const char* pchName, _HotKey hk);

	bool CreateTable();
	void InitHotKey();
};