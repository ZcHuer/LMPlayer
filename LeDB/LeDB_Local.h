#pragma once
#include "LeDB.h"

class CLeDB_Local : public ILeDB_Local
{
public:
	CLeDB_Local();
	~CLeDB_Local();

	// ILeDB_Local
	bool InsertLocalFileList(const char* pchMd5, const char* pchPath, const char* pchName, long currPlayTime, long totalTime, long bError);
	bool UpdateLocalFileList(const char* pchMd5, long currPlayTime, long totalTime, long bError);
	bool SelectLocalFileList(void** pVoid_Local, bool bLimit = false);
	char* SelectPath(const char* pchMd5);
	long GetPlayProcess_ByMd5_Local(const char* pchMd5);
	bool DelLocalFileItem(const char* pchMd5);
	bool DelLocalFileList();

	bool CreateTable();
};