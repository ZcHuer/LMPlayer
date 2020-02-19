#include "stdafx.h"
#include "MakeZip.h"
#include "..\..\baseclass\Base.h"

CMakeZip::CMakeZip(void) : m_zip(NULL), m_pfnCB(NULL), m_hAbortEvent(NULL)
{

}

CMakeZip::CMakeZip(PFNZipCallback pfn) : m_zip(NULL), m_pfnCB(pfn), m_hAbortEvent(NULL)
{

}

CMakeZip::~CMakeZip(void)
{
	ZipClose();
}

void CMakeZip::SetAbortEnvent(HANDLE hEvent)
{
	m_hAbortEvent = hEvent;
}

bool CMakeZip::IsAbort()
{
	if (m_hAbortEvent)
		return WAIT_OBJECT_0 == WaitForSingleObject(m_hAbortEvent, 0);

	return false;
}

BOOL CMakeZip::ZipCreate(LPCWSTR lpwsZipFileName)
{
	if(NULL == lpwsZipFileName)
		return FALSE;

	if(m_zip)
	{
		ATLASSERT(FALSE);
		ZipClose();
	}

	HELPER::MakeDirExist(lpwsZipFileName, true);

	m_zip = CreateZip(lpwsZipFileName, NULL);
	return m_zip != NULL;
}

void CMakeZip::ZipClose()
{
	if(m_zip)
	{
		CloseZip(m_zip);
		m_zip = NULL;
	}
}

BOOL CMakeZip::ZipAddFile(LPCWSTR lpwsFile, LPCWSTR lpwsName)
{
	if(NULL == m_zip || NULL == lpwsFile || !PathFileExists(lpwsFile))
		return FALSE;

	if(m_pfnCB)
	{
		BOOL bContinue = TRUE;
		WIN32_FIND_DATA fd;
		HANDLE hFind = FindFirstFile(lpwsFile, &fd);
		if(hFind != INVALID_HANDLE_VALUE)
		{
			bContinue = m_pfnCB(lpwsFile, &fd);
			FindClose(hFind);
		}
		if(!bContinue)
		{
			ZipAddFakeFile(lpwsName);
			return TRUE;
		}
	}

	if(NULL == lpwsName || lpwsName[0] == L'\0')
	{
		lpwsName = PathFindFileName(lpwsFile);
	}

	if(ZR_OK == ::ZipAdd(m_zip, lpwsName, lpwsFile))
		return TRUE;

	return ZipAddFileMemery(lpwsFile, lpwsName);
}

BOOL CMakeZip::ZipAddFolder(LPCWSTR lpwsFolder, LPCWSTR lpwsName)
{
	if(NULL == m_zip || NULL == lpwsFolder || !PathFileExists(lpwsFolder))
		return FALSE;

	if(NULL != lpwsName && lpwsName[0] != L'\0')
		if(ZR_OK != ::ZipAddFolder(m_zip, lpwsName))
			return FALSE;

	CString strFolder(lpwsFolder);
	PathAddBackslash(strFolder.GetBufferSetLength(MAX_PATH));
	strFolder.ReleaseBuffer();

	CString strName(lpwsName?lpwsName:L"");
	if(!strName.IsEmpty())
	{
		PathAddBackslash(strName.GetBufferSetLength(MAX_PATH));
		strName.ReleaseBuffer();
	}

	WIN32_FIND_DATA fd = {0};
	HANDLE hFind = FindFirstFile(strFolder + L"*", &fd);
	if(INVALID_HANDLE_VALUE == hFind)
		return FALSE;

	BOOL bRtn = TRUE;//Maybe Empty Folder
	do 
	{
		if(StrCmpI(fd.cFileName, L".") == 0
			|| StrCmpI(fd.cFileName, L"..") == 0)
			continue;

		if(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN 
			|| fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			continue;

		if(m_pfnCB && !m_pfnCB(strFolder + fd.cFileName, &fd))
		{
			BOOL bRtn1 = ZipAddFakeFile(strName + fd.cFileName);
			ATLASSERT(bRtn1);
			bRtn = bRtn && bRtn1;
			continue;
		}

		if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			BOOL bRtn1 = ZipAddFolder(strFolder + fd.cFileName, strName + fd.cFileName);
			ATLASSERT(bRtn1);
			bRtn = bRtn && bRtn1;
		}
		else
		{
			BOOL bRtn1 = ZipAddFileNoCheck(strFolder + fd.cFileName, strName + fd.cFileName);
			ATLASSERT(bRtn1);
			bRtn = bRtn && bRtn1;
		}
	} while (FindNextFile(hFind, &fd) && !IsAbort());
	FindClose(hFind);
	return bRtn;
}

BOOL CMakeZip::ZipAddFakeFile(LPCWSTR lpwsName)
{
	ATLASSERT(lpwsName);

	CString strFakeName(lpwsName);
	LPBYTE pbyFake[16] = {0};
	return ZR_OK == ZipAdd(m_zip, strFakeName + L"#", pbyFake, sizeof(pbyFake));
}


BOOL CMakeZip::ZipAddFileNoCheck(LPCWSTR lpwsFile, LPCWSTR lpwsName)
{
	if(NULL == m_zip || NULL == lpwsFile || !PathFileExists(lpwsFile))
		return FALSE;

	if(NULL == lpwsName || lpwsName[0] == L'\0')
	{
		lpwsName = PathFindFileName(lpwsFile);
	}

	if(ZR_OK == ::ZipAdd(m_zip, lpwsName, lpwsFile))
		return TRUE;

	return ZipAddFileMemery(lpwsFile, lpwsName);
}

#define KB		1024
#define MB		1024*KB
#define GB		1024*MB

BOOL CMakeZip::ZipAddFileMemery(LPCWSTR lpFile, LPCWSTR lpwsName)
{
	HANDLE hFile = CreateFile(lpFile, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(INVALID_HANDLE_VALUE == hFile)
		return FALSE;

	DWORD dwSizeHigh = 0;
	DWORD dwSizeLow = GetFileSize(hFile, &dwSizeHigh);

	BOOL bRtn = FALSE;
	if(dwSizeHigh != 0 || INVALID_FILE_SIZE == dwSizeLow)
	{
		bRtn = ZipAddFakeFile(lpwsName);
	}
	else
	{
		if(dwSizeLow > 20 * MB)// >20MB
		{
			LPBYTE pbyBuf = new BYTE[5*MB];
			if(ReadFile(hFile, pbyBuf, dwSizeLow, &dwSizeHigh, NULL))
			{
				bRtn = ZR_OK == ZipAdd(m_zip, lpwsName, pbyBuf, dwSizeHigh);
			}
			if(bRtn)
			{
				LPBYTE pbyBuf = new BYTE[15*MB];
				SetFilePointer(hFile, -15*MB, NULL, FILE_END);
				if(ReadFile(hFile, pbyBuf, dwSizeLow, &dwSizeHigh, NULL))
				{
					bRtn = ZR_OK == ZipAdd(m_zip, lpwsName, pbyBuf, dwSizeHigh);
				}
				delete [] pbyBuf;
			}
			delete [] pbyBuf;
		}
		else
		{
			if(dwSizeLow == 0)
			{
				bRtn = TRUE;
				ZipAdd(m_zip, lpwsName, &bRtn, dwSizeLow);
			}
			else
			{
				LPBYTE pbyBuf = new BYTE[dwSizeLow];
				if(ReadFile(hFile, pbyBuf, dwSizeLow, &dwSizeHigh, NULL))
				{
					bRtn = ZR_OK == ZipAdd(m_zip, lpwsName, pbyBuf, dwSizeHigh);
				}
				delete [] pbyBuf;
			}
		}
	}

	CloseHandle(hFile);

	return bRtn;
}
