#pragma once
#include "zip.h"

typedef BOOL (__stdcall * PFNZipCallback)(LPCWSTR lpwsFile, LPWIN32_FIND_DATA lpFindData);//return TRUE , continue Add File

class CMakeZip
{
public:
	CMakeZip(void);
	CMakeZip(PFNZipCallback pfn);
	~CMakeZip(void);

public:
	BOOL ZipCreate(LPCWSTR lpwsZipFileName);
	void ZipClose();
	
	BOOL ZipAddFile(LPCWSTR lpFile, LPCWSTR lpwsName);
	BOOL ZipAddFolder(LPCWSTR lpForder, LPCWSTR lpwsName); 
	BOOL ZipAddFakeFile(LPCWSTR lpwsName);

	void SetAbortEnvent(HANDLE hEvent);
	bool IsAbort();

private:
	BOOL ZipAddFileNoCheck(LPCWSTR lpFile, LPCWSTR lpwsName);
	BOOL ZipAddFileMemery(LPCWSTR lpFile, LPCWSTR lpwsName);
	
private:
	HZIP m_zip;
	PFNZipCallback m_pfnCB;
	HANDLE m_hAbortEvent;
};
