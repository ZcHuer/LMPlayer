#ifndef __INI_HELPER_H__
#define __INI_HELPER_H__

#include <string>
#include <windows.h>

#pragma once

class IniHelper {
public:
    IniHelper(LPCWSTR lpcsIniFile);
    ~IniHelper();

    bool SetInt( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, int nValue );
    int GetInt( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, int nDefault = 0 );

    bool SetInt64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LONGLONG nValue );
    LONGLONG GetInt64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LONGLONG nDefault = 0 );

    bool SetUint64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, ULONGLONG nValue );
    ULONGLONG GetUint64( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, ULONGLONG nDefault = 0 );

    bool SetBool( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, bool bValue );
    bool GetBool( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, bool bDefault = false );

    bool SetString( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LPCWSTR lpcsValue );
    std::wstring GetString( LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName, LPCWSTR lpcsDefault = L"" );

    bool DeleteSection( const LPCWSTR lpcsSectionName );
    bool DeleteSectionKey( const LPCWSTR lpcsSectionName, LPCWSTR lpcsKeyName );

private:
    wchar_t m_csIniFile[MAX_PATH];
};


IniHelper* TestModeHelper();            // APPDATA/51Talk/TestMode.ini
IniHelper* ConfigHelper();              // APPDATA/51Talk/Config.ini
IniHelper* SubConfigHelper();           // APPDATA/51Talk/Config/Config.ini
IniHelper* CommonConfigHelper();        // COMMON_APPDATA/51Talk/Config.ini
IniHelper* B2SConfigHelper();           // APPDATA/51Talk/config/b2s.ini


#endif
