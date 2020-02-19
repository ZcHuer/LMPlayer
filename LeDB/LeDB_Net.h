#pragma once
#include "LeDB.h"

class CLeDB_Net : public ILeDB_Net
{
public:
	CLeDB_Net();
	~CLeDB_Net();

	// ILeDB_Net
	bool InsertPlayRecodEx(int nType, long lcpid, long conAlbumId, long lCurrPlayTime, long lTotalTime, long orderNumber, const char* pchContentName, long time, const char* pchUrl = "", const char* pchRemarks = "");
	bool UpdatePlayRecodEx(long conAlbumId, long lCurrPlayTime, long lTotalTime, long orderNumber, const char* pchContentName, const char* pchUrl = "", const char* pchRemarks = "");
	bool SelectPlayRecodEx(void** pVoid_Net, bool bLimit = false);
	bool DelPlayRecodEx(long conAlbumId);
	long GetPlayProcess_ById_NetVideoEx(long conAlbumId, int nindex);

	bool CreateTable();
};