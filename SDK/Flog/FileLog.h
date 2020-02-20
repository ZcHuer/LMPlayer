#pragma once
#include <Windows.h>
#include <atlstr.h>

#ifndef LOGINIT
#define LOGINIT(path,...) {CFileLog::InitPath(path,##__VA_ARGS__);}
#endif // !LOGINIT

#ifndef LOGINIT_Data
#define LOGINIT_Data(path,...) {CFileLog::InitPath_Data(path,##__VA_ARGS__);}
#endif // !LOGINIT_Data

#ifndef FLOG
#define FLOG(lpsFormat,...) {CFileLog fLog; fLog.Log(lpsFormat,##__VA_ARGS__);}
#endif // !FLOG

#ifndef FLOG_Data
#define FLOG_Data(lpsFormat,...) {CFileLog fLog; fLog.Log_Data(lpsFormat,##__VA_ARGS__);}
#endif // !FLOG

class CFileLog
{
public:
	CFileLog();
	~CFileLog();

	static void InitPath(LPCTSTR lpsFormat,...);
	static void InitPath_Data(LPCTSTR lpsFormat, ...);
	void Log(LPCTSTR lpsFormat,...);
	void Log_Data(LPCTSTR lpsFormat, ...);
protected:
	void WriteData(LPCTSTR lpData);
	void WriteData_Data(LPCTSTR lpData);

private:
	static CRITICAL_SECTION m_csLog;
	static CFileLog* _this;
	static CString m_pLogPath;
	static CRITICAL_SECTION m_csLog_Data;
	static CString m_pLogPath_Data;
};

