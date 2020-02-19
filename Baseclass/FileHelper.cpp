#include "stdafx.h"
#include "FileHelper.h"
#include "LogBase.h"

namespace FileHelper {
	BOOL WriteFileLog(LPCWSTR lpstrFile, LPCWSTR lpstrContent)
	{
		if (NULL == lpstrFile || NULL == lpstrContent)
			return FALSE;

		if(PathFileExists(lpstrFile))
			DeleteFile(lpstrFile);

		BOOL bRtn = HELPER::MakeDirExist(lpstrFile, true);
		ATLASSERT(bRtn);

		CLogBase log;
		log.InitLog(lpstrFile);
		log.WriteLogExW(lpstrContent);
		log.UninitLog();

		return TRUE;
	}
}