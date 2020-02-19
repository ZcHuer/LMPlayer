#include "../LePlayer/stdafx.h"
#include "CommandParse.h"

CCommandParse::CCommandParse(void)
{

}

CCommandParse::CCommandParse(CCommandParse& parse)
{
	for(DWORD i = 0; i < parse.m_CommandArray.size(); ++i)
	{
		CCommand* pParseCmd = parse.m_CommandArray.at(i);
		if(pParseCmd)
		{
			CCommand* pCmd = new CCommand(pParseCmd);
			ATLASSERT(pCmd);
			m_CommandArray.push_back(pCmd);
		}
		else
		{
			//todo ???
		}		
	}
	for(DWORD i = 0; i < parse.m_FileArray.size(); ++i)
	{
		m_FileArray.push_back(parse.m_FileArray.at(i));
	}
	m_strCommandLine = parse.m_strCommandLine;
}

CCommandParse& CCommandParse::operator =(CCommandParse& parse)
{
	if(this == &parse)
		return *this;

	for(DWORD i = 0; i < parse.m_CommandArray.size(); ++i)
	{
		CCommand* pParseCmd = parse.m_CommandArray.at(i);
		if(pParseCmd)
		{
			CCommand* pCmd = new CCommand(pParseCmd);
			ATLASSERT(pCmd);
			m_CommandArray.push_back(pCmd);
		}
		else
		{
			//todo ???
		}		
	}
	for(DWORD i = 0; i < parse.m_FileArray.size(); ++i)
	{
		m_FileArray.push_back(parse.m_FileArray.at(i));
	}
	m_strCommandLine = parse.m_strCommandLine;
	return *this;
}

CCommandParse::~CCommandParse(void)
{
	Clear();
}

BOOL CCommandParse::Parse(LPCWSTR lpwsCommandLine)
{
	if(NULL == lpwsCommandLine || lstrlenW(lpwsCommandLine) <= 0)
		return FALSE;

	CString strWCMDLine(lpwsCommandLine);
	m_strCommandLine = lpwsCommandLine;

	const DWORD dwLength = strWCMDLine.GetLength();
	LPCWSTR pwsCmdLine = strWCMDLine;

	//WCHAR wszCommand[64] = {0};
	WCHAR* pwszCommand = NULL;
	//WCHAR wszString[MAX_PATH] = {0};
	WCHAR* pwszString = NULL;

	CCommand* pCmd = NULL;

	for(DWORD i = 0; i < dwLength;)
	{
		DWORD dwSkip = 0;
		if(0 == pwsCmdLine[i])//到达结尾
		{		
			i = dwLength;//可以退出
			continue;
		}

		if(L'/' == pwsCmdLine[i] || L'-' == pwsCmdLine[i])//命令开始
		{
			if(pwszCommand)
				delete [] pwszCommand;
			int nLength = lstrlenW(pwsCmdLine+i);
			nLength = nLength > MAX_PATH ? nLength : MAX_PATH;
			pwszCommand = new WCHAR[nLength];
			ATLASSERT(pwszCommand);
			memset(pwszCommand, 0, sizeof(WCHAR)*nLength);

			dwSkip = ParseCommand(pwsCmdLine+i, pwszCommand, nLength-1);
			if(dwSkip > 0)//添加到结构
			{
				i += dwSkip;

				pCmd = new CCommand();
				ATLASSERT(pCmd!=NULL);

				pCmd->SetCommand(pwszCommand);
				AddCommand(pCmd);
			}
		}
		else if(L'"' == pwsCmdLine[i])//显式字符串开始
		{
			if(pwszString)
				delete [] pwszString;
			int nLength = lstrlenW(pwsCmdLine+i);
			nLength = (nLength > MAX_PATH ? nLength : MAX_PATH);
			pwszString = new WCHAR[nLength];
			ATLASSERT(pwszString);
			memset(pwszString, 0, sizeof(WCHAR)*nLength);

			dwSkip = ParseArgs(pwsCmdLine+i, pwszString, nLength-1, FALSE);
			if(dwSkip > 0)
			{
				i += dwSkip;

				if( pwszString[0] != 0 )
				{
					if(pCmd)
						pCmd->AddArg(pwszString);
					else
						m_FileArray.push_back(CString(pwszString));
				}
			}
		}
		else if(L' ' == pwsCmdLine[i])//分隔符
		{
			i += 1;
		}
		else//应该是其他字符，隐式字符串
		{
			if(pwszString)
				delete [] pwszString;
			int nLength = lstrlenW(pwsCmdLine+i);
			nLength = (nLength > MAX_PATH ? nLength : MAX_PATH);
			pwszString = new WCHAR[nLength];
			ATLASSERT(pwszString);
			memset(pwszString, 0, sizeof(WCHAR)*nLength);

			dwSkip = ParseArgs(pwsCmdLine+i, pwszString, nLength-1, TRUE);
			if(dwSkip > 0)
			{
				i += dwSkip;

				if(pCmd)
					pCmd->AddArg(pwszString);
				else
					m_FileArray.push_back(CString(pwszString));
			}
		}
	}

	if(pwszCommand)
		delete [] pwszCommand;
	if(pwszString)
		delete [] pwszString;

	return TRUE;
}

LPCWSTR CCommandParse::GetCommandLineString()
{
	return m_strCommandLine;
}

size_t CCommandParse::GetCommandCount()
{
	return m_CommandArray.size();
}

CCommand* CCommandParse::GetCommand(size_t dwIndex)
{
	if(dwIndex >= m_CommandArray.size())
		return NULL;

	return m_CommandArray[dwIndex];
}

DWORD CCommandParse::ParseCommand(LPCWSTR lpwsCommand, LPWSTR lpwszBuf, DWORD dwSize)
{
	if(L'-' != lpwsCommand[0] && L'/' != lpwsCommand[0])
		return 0;

	DWORD dwSkip = 1;
	DWORD dwIndex = 0;
	const DWORD dwLength = lstrlenW(lpwsCommand);

	for(DWORD i = 1; i < dwLength && dwIndex < dwSize-1; ++i, ++dwSkip)
	{
		if(L'=' == lpwsCommand[i] || L' ' == lpwsCommand[i])
		{
			lpwszBuf[dwIndex] = 0;
			++dwSkip;
			break;
		}
		lpwszBuf[dwIndex++] = lpwsCommand[i];
	}

	lpwszBuf[dwIndex] = 0;

	return dwSkip;
}

DWORD CCommandParse::ParseArgs(LPCWSTR lpwsCommand, LPWSTR lpwszBuf, DWORD dwSize, BOOL bSpace)
{
	DWORD dwSkip = (bSpace ? 0 : 1);
	DWORD dwIndex = 0;
	const DWORD dwLength = lstrlenW(lpwsCommand);

	for(DWORD i = (bSpace ? 0 : 1); i < dwLength && dwIndex < dwSize-1; ++i, ++dwSkip)
	{
		if((bSpace && L' ' == lpwsCommand[i]))
		{
			++dwSkip;
			lpwszBuf[dwIndex] = 0;
			break;
		}
		else if(L'"' == lpwsCommand[i])
		{
			if(i + 1 >= dwLength)//当前是最后一个字符
			{
				++dwSkip;
				lpwszBuf[dwIndex] = 0;
				break;
			}

			//此时“"”后面还有其他字符，判断是不是命令分隔符号“ ”（空格）， 是则当前参数结束，否则认识是参数字符串的内容，参数串还没有结束
			if(lpwsCommand[i+1] == L' ')//参数没有后续字符了
			{
				++dwSkip;
				lpwszBuf[dwIndex] = 0;
				break;
			}
		}
		lpwszBuf[dwIndex++] = lpwsCommand[i];
	}

	lpwszBuf[dwIndex] = 0;

	return dwSkip;
}

size_t CCommandParse::GetFileCount()
{
	return m_FileArray.size();
}

LPCWSTR CCommandParse::GetFile(size_t dwIndex)
{
	if(dwIndex >= m_FileArray.size())
		return NULL;

	return m_FileArray[dwIndex];
}

void CCommandParse::AddCommand(CCommand* lpCommand)
{
	if(NULL == lpCommand)
		return ;

	m_CommandArray.push_back(lpCommand);
}

void CCommandParse::Clear()
{
	for(DWORD i = 0; i < m_CommandArray.size(); ++i)
	{
		CCommand* pCmd = m_CommandArray[i];
		if(pCmd)
			delete pCmd;
	}
	m_FileArray.clear();
	m_CommandArray.clear();
}

CCommand* CCommandParse::FindCommand(LPCWSTR lpSearchCmd)
{
	for(DWORD i = 0; i < m_CommandArray.size(); ++i)
	{
		CCommand* pCmd = m_CommandArray[i];
		if(pCmd && pCmd->GetCommand() )
		{
			if(lstrcmpiW(lpSearchCmd, pCmd->GetCommand() ) == 0 )
			{
				return pCmd;
			}
		}
	}

	return NULL;
}

//=================================================================================================
CCommand::CCommand()
{

}

CCommand::CCommand(CCommand* pCmd)
{
	if(pCmd)
	{
		for(DWORD i = 0; i < pCmd->m_Args.size(); ++i)
		{
			m_Args.push_back(pCmd->m_Args.at(i));
		}
		m_strCommand = pCmd->m_strCommand;
	}
}

CCommand::CCommand(CCommand& cmd)
{
	for(DWORD i = 0; i < cmd.m_Args.size(); ++i)
	{
		m_Args.push_back(cmd.m_Args.at(i));
	}
	m_strCommand = cmd.m_strCommand;
}

CCommand::~CCommand()
{
	Clear();
}

void CCommand::SetCommand(LPCWSTR lpwsCommand)
{
	if(NULL == lpwsCommand)
		return ;

	if(lpwsCommand == 0)
		return;

	m_strCommand = lpwsCommand;

	//改为小写。
	m_strCommand.MakeLower();
}

LPCWSTR CCommand::GetCommand()
{
	return m_strCommand;
}

size_t CCommand::GetArgCount()
{
	return m_Args.size();
}

void CCommand::AddArg(LPCWSTR lpwsArg)
{
	if(NULL == lpwsArg)
		return ;

	DWORD dwLength = lstrlenW(lpwsArg);
	if(dwLength <= 0)
		return;

	m_Args.push_back(CString(lpwsArg));
}

LPCWSTR CCommand::GetArg(size_t dwIndex)
{
	if(dwIndex >= m_Args.size())
		return NULL;

	return m_Args[dwIndex];
}

void CCommand::Clear()
{
	m_Args.clear();
}
