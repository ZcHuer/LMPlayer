#include <UserEnv.h>
#pragma comment(lib,"Userenv.lib")
//关于用户权限Sid 转换
#include <sddl.h>
#pragma comment(lib,"Advapi32.lib")
#include <TlHelp32.h>
#include <string>
#include <strsafe.h>
using namespace std;
/****
*服务写入当前用户的注册表
*****/
BOOL GetTokenByName(HANDLE &hToken, LPTSTR lpName)
{
	if (!lpName)
		return FALSE;

	HANDLE         hProcessSnap = NULL;
	BOOL           bRet = FALSE;
	PROCESSENTRY32 pe32 = { 0 };

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return (FALSE);

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap, &pe32))
	{
		do
		{
			if (!_tcscmp(_tcsupr(pe32.szExeFile), _tcsupr(lpName)))
			{
				HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
				bRet = OpenProcessToken(hProcess, TOKEN_ALL_ACCESS, &hToken);
				CloseHandle(hProcess);
				CloseHandle(hProcessSnap);
				return (bRet);
			}
		} while (Process32Next(hProcessSnap, &pe32));
		bRet = FALSE;
	}
	else
	{
		bRet = FALSE;
	}

	CloseHandle(hProcessSnap);
	return (bRet);
}

//获取用户sid
bool GetAccountSid(LPTSTR AccountName, PSID *Sid)
{
	PSID pSID = NULL;
	DWORD cbSid = 0;
	LPTSTR DomainName = NULL;
	DWORD cbDomainName = 0;
	SID_NAME_USE SIDNameUse;
	BOOL  bDone = FALSE;
	try
	{
		if (!LookupAccountName(NULL,
			AccountName,
			pSID,
			&cbSid,
			DomainName,
			&cbDomainName,
			&SIDNameUse))
		{
			pSID = (PSID)malloc(cbSid);
			DomainName = (LPTSTR)malloc(cbDomainName * sizeof(TCHAR));
			if (!pSID || !DomainName)
			{
				throw;
			}
			if (!LookupAccountName(NULL,
				AccountName,
				pSID,
				&cbSid,
				DomainName,
				&cbDomainName,
				&SIDNameUse))
			{
				throw;
			}
			bDone = TRUE;
		}
	}
	catch (...)
	{
		//nothing
	}
	if (DomainName)
	{
		free(DomainName);
	}

	if (!bDone && pSID)
	{
		free(pSID);
	}
	if (bDone)
	{
		*Sid = pSID;
	}
	return bDone;
}
void WriteAppCurrentReg(string path)
{
	HANDLE hToken = NULL;
	do
	{
		if (!GetTokenByName(hToken, _T("EXPLORER.EXE")))
		{
			break;
		}

		// 模拟登录用户的安全上下文
		if (FALSE == ImpersonateLoggedOnUser(hToken))
		{
			break;
		}
		//获取用户名
		TCHAR szUsername[MAX_PATH];
		DWORD dwUsernameLen = MAX_PATH;
		if (FALSE == GetUserName(szUsername, &dwUsernameLen))
			break;

		//到这里已经模拟完了，别忘记返回原来的安全上下文
		if (FALSE == RevertToSelf())
			break;

		// 获取sid
		PSID pSid = NULL;
		LPWSTR sid;
		GetAccountSid(szUsername, &pSid); //获取得到的是一个结构体
		ConvertSidToStringSid(pSid, &sid); //从结构体中得到sid串		
		HKEY hKey;
		int i = 0;    //操作结果：0==succeed

		wchar_t lswRegValue[MAX_PATH];
		StringCchPrintf(lswRegValue, MAX_PATH, L"%s", LeTools::s2ws(path).c_str());

		wchar_t lswKeyPath[MAX_PATH] = { 0 };
		StringCchPrintf(lswKeyPath, MAX_PATH, L"%s\\Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\LMPlayer", sid);
		if (RegOpenKeyEx(HKEY_USERS, lswKeyPath, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			if (RegSetValueEx(hKey, L"", NULL, REG_SZ, (const LPBYTE)lswRegValue, (wcslen(lswRegValue) + 1) * sizeof(wchar_t)) != ERROR_SUCCESS)
			{
				i = 1;
			}
			RegCloseKey(hKey);
		}
		else
		{
			i = 1;
		}

	} while (false);
	if (NULL != hToken)
		CloseHandle(hToken);
}