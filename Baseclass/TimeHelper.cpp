#include "stdafx.h"
#include "TimeHelper.h"
#include <time.h>

BOOL ToSystemTimeFormat(ULONGLONG nUTC, LPSYSTEMTIME lpST)
{
	if(NULL == lpST)
		return FALSE;
	
	ULARGE_INTEGER ul;
	ul.QuadPart = nUTC;
	FILETIME ft;
	ft.dwHighDateTime = ul.HighPart;
	ft.dwLowDateTime = ul.LowPart;

	return FileTimeToSystemTime(&ft, lpST);
}

ULONGLONG ToTimeZone(ULONGLONG nUTC, int nZone)
{
	ULONGLONG utc = nUTC;
	if(nZone > 0)
	{
		utc += (ULONGLONG)nZone * HOUR_NANOSECOND100;
	}
	else if( nZone < 0)
	{
		utc -= ((ULONGLONG)(-nZone)) * HOUR_NANOSECOND100;
	}
	return utc;
}

ULONGLONG FromCRTTime(ULONGLONG uTime)
{
	//uTime : 1970.01.01 00:00:00	到此时的秒数

	//计算基准时间
	SYSTEMTIME st = { 1970,1,0,1,0,0,0,0 };
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);

	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;
	ul.QuadPart += (ULONGLONG)uTime*SECOND_NANOSECOND100;

	return ul.QuadPart;
}

ULONGLONG ScanTimeString(LPCWSTR lpwsTimeString)
{
	if(NULL == lpwsTimeString)
		return 0;

	SYSTEMTIME st = {0};
	int n = swscanf_s(lpwsTimeString, L"%hd-%hd-%hd %hd:%hd:%hd", 
		&st.wYear, 
		&st.wMonth,
		&st.wDay,
		&st.wHour,
		&st.wMinute,
		&st.wSecond);

	if(n != 6)
		return 0;
	
	FILETIME ft = {0};
	if(!SystemTimeToFileTime(&st, &ft))
		return 0;
	
	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;
	return (ULONGLONG)ul.QuadPart;
}

ULONGLONG ToCRTTime(ULONGLONG uTime)
{
	//116444736000000000
	/*
	SYSTEMTIME st = {1970,1,0,1,0,0,0,0};
	FILETIME ft;
	SystemTimeToFileTime(&st, &ft);
	ULARGE_INTEGER ul;
	ul.HighPart = ft.dwHighDateTime;
	ul.LowPart = ft.dwLowDateTime;
	116444736000000000 <===> ul.QuadPart;
	*/
	static const ULONGLONG s_timespan = 116444736000000000;
	if(uTime < s_timespan)
	{
		ATLASSERT(FALSE);
		return s_timespan;
	}

	ULONGLONG uCRTTime = uTime - s_timespan;//变更到1970年
	uCRTTime /= SECOND_NANOSECOND100;//计算到秒
	return uCRTTime;
}

LPCWSTR GetMonthEN(DWORD dwMonth)
{
	static LPCWSTR s_szMonthEnglish[] = {
		L"",
		L"January",
		L"February",
		L"March",
		L"April",
		L"May",
		L"June",
		L"July",
		L"August",
		L"September",
		L"October",
		L"November",
		L"December",
	};

	if(dwMonth <= 0 || dwMonth > 12)
		return L"";

	return s_szMonthEnglish[dwMonth];
}

BOOL IsSameDay(ULONGLONG uTime1, ULONGLONG uTime2)
{
	uTime1 /= (ULONGLONG)24*HOUR_NANOSECOND100;//天
	uTime2 /= (ULONGLONG)24*HOUR_NANOSECOND100;//天
	return uTime1 == uTime2;
}

BOOL IsToday(ULONGLONG uTime, ULONGLONG uNowUTC)
{
	return IsSameDay(uTime, uNowUTC);
}

BOOL IsTomorrow(ULONGLONG uTime, ULONGLONG uNowUTC)
{
	uNowUTC += (ULONGLONG)24*HOUR_NANOSECOND100;//Tomorrow
	return IsSameDay(uTime, uNowUTC);
}

BOOL IsAfterTomorrow(ULONGLONG uTime, ULONGLONG uNowUTC)
{
	uNowUTC += (ULONGLONG)48*HOUR_NANOSECOND100;//AfterTomorrow
	return IsSameDay(uTime, uNowUTC);
}

time_t SystemTime2Time_t(SYSTEMTIME& systime)
{
	struct tm temptm = { systime.wSecond,
		systime.wMinute,
		systime.wHour,
		systime.wDay,
		systime.wMonth - 1,
		systime.wYear - 1900,
		systime.wDayOfWeek,
		0,
		0 };
	time_t time = mktime(&temptm);

	return time;
}

LPCWSTR GetDayEN(DWORD dwDay)
{
	static LPCWSTR s_szDayEnglish[] = {
		L"",
		L"1st",
		L"2nd",
		L"3rd",
		L"4th",
		L"5th",
		L"6th",
		L"7th",
		L"8th",
		L"9th",
		L"10th",
		L"11th",
		L"12th",
		L"13th",
		L"14th",
		L"15th",
		L"16th",
		L"17th",
		L"18th",
		L"19th",
		L"20th",
		L"21st",
		L"22nd",
		L"23rd",
		L"24th",
		L"25th",
		L"26th",
		L"27th",
		L"28th",
		L"29th",
		L"30th",
		L"31st",
	};

	if(dwDay <= 0 || dwDay > 31)
		return L"";

	return s_szDayEnglish[dwDay];
}


LPCWSTR GetWeekDay( DWORD dwWeek )
{
	switch (dwWeek)
	{
	case 1:
		return L"Monday";
	case 2:
		return L"Tuesday";
	case 3:
		return L"Wednesday";
	case 4:
		return L"Thursday";
	case 5:
		return L"Friday";
	case 6:
		return L"Saturday";
	case 0:
		return L"Sunday";
	default:
		break;
	}
	ATLASSERT(FALSE);
	return L"";
}

LPCWSTR GetWeekDayUpWord( DWORD dwWeek )
{
	switch (dwWeek)
	{
	case 1:
		return L"一";
	case 2:
		return L"二";
	case 3:
		return L"三";
	case 4:
		return L"四";
	case 5:
		return L"五";
	case 6:
		return L"六";
	case 0:
		return L"日";
	default:
		break;
	}
	ATLASSERT(FALSE);
	return L"";
}
