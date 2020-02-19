#pragma once
#include <vector>
class CCommand;
typedef std::vector<CCommand*> CommandArray;
typedef std::vector<CString> StringArray;

class CCommandParse
{
public:
	CCommandParse(); 
	CCommandParse(CCommandParse& parse);
	CCommandParse& operator =(CCommandParse& parse);
	virtual ~CCommandParse();

public:
	virtual BOOL Parse(LPCWSTR lpwsCommandLine);

public:
	LPCWSTR GetCommandLineString();

	size_t GetCommandCount();
	CCommand* GetCommand(size_t dwIndex);

	size_t GetFileCount();
	LPCWSTR GetFile(size_t dwIndex);

	CCommand* FindCommand(LPCWSTR lpSearchCmd);

	void Clear();

private:
	DWORD ParseCommand(LPCWSTR lpwsCommand, LPWSTR lpwszBuf, DWORD dwSize);
	DWORD ParseArgs(LPCWSTR lpwsCommand, LPWSTR lpwszBuf, DWORD dwSize, BOOL bSpace);

private:
	void AddCommand(CCommand* lpCommand);

protected:
	CommandArray m_CommandArray;
	StringArray m_FileArray;

	CString m_strCommandLine;
};

class CCommand
{
public:
	CCommand();
	CCommand(CCommand* pCmd);
	CCommand(CCommand& cmd);
	virtual ~CCommand();

public:
	void SetCommand(LPCWSTR lpwsCommand);
	LPCWSTR GetCommand();

	size_t GetArgCount();
	void AddArg(LPCWSTR lpwsArg);
	LPCWSTR GetArg(size_t dwIndex);

private:
	void Clear();

private:
	CString m_strCommand;
	StringArray m_Args;
};
