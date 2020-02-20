#include "FileLog.h"
#include <string>
using namespace std;

CRITICAL_SECTION CFileLog::m_csLog;
CRITICAL_SECTION CFileLog::m_csLog_Data;
CString CFileLog::m_pLogPath = _T("");
CString CFileLog::m_pLogPath_Data = _T("");
CFileLog* CFileLog::_this = nullptr;
CFileLog::CFileLog()
{
	InitializeCriticalSection(&m_csLog);
	InitializeCriticalSection(&m_csLog_Data);
	_this = this;
}

CFileLog::~CFileLog()
{
}

void CFileLog::InitPath(LPCTSTR lpsFormat, ...)
{
	CString strFormat;
	va_list args;
	va_start(args, lpsFormat);
	strFormat.FormatV(lpsFormat, args);
	ATLTRACE(strFormat);
	m_pLogPath = strFormat;
}

void CFileLog::InitPath_Data(LPCTSTR lpsFormat, ...)
{
	CString strFormat;
	va_list args;
	va_start(args, lpsFormat);
	strFormat.FormatV(lpsFormat, args);
	ATLTRACE(strFormat);
	m_pLogPath_Data = strFormat;
}

void CFileLog::Log(LPCTSTR lpsFormat, ...)
{
	EnterCriticalSection(&m_csLog);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString strFormat;
	strFormat.Format(_T("%d-%02d-%02d %02d:%02d:%02d[%03d] data:"),sysTime.wYear, sysTime.wMonth, sysTime.wDay, 
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	ATLTRACE(strFormat);

	_this->WriteData(strFormat);

	va_list args;
	va_start(args, lpsFormat);
	strFormat.FormatV(lpsFormat, args);
	ATLTRACE(strFormat);

	_this->WriteData(strFormat);

	_this->WriteData(_T("\r\n"));

	LeaveCriticalSection(&m_csLog);
}

void CFileLog::Log_Data(LPCTSTR lpsFormat, ...)
{
	EnterCriticalSection(&m_csLog_Data);
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);

	CString strFormat;
	strFormat.Format(_T("%d-%02d-%02d %02d:%02d:%02d[%03d] data:"), sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond, sysTime.wMilliseconds);
	ATLTRACE(strFormat);

	_this->WriteData_Data(strFormat);

	va_list args;
	va_start(args, lpsFormat);
	strFormat.FormatV(lpsFormat, args);
	ATLTRACE(strFormat);

	_this->WriteData_Data(strFormat);

	_this->WriteData_Data(_T("\r\n"));

	LeaveCriticalSection(&m_csLog_Data);
}

void CFileLog::WriteData(LPCTSTR lpData)
{
	string szData = CT2CA(lpData);
    FILE * file;
    fopen_s(&file,CT2CA(m_pLogPath), "ab+");
	if (file){
		fwrite(szData.data(), szData.length(), 1, file);
		fclose(file);
	}
}

void CFileLog::WriteData_Data(LPCTSTR lpData)
{
	string szData = CT2CA(lpData);
	FILE * file;
	fopen_s(&file, CT2CA(m_pLogPath_Data), "ab+");
	if (file) {
		fwrite(szData.data(), szData.length(), 1, file);
		fclose(file);
	}
}
