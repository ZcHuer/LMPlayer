#pragma once
#include "PipeStream.h"

class CPipe
{
public:
	CPipe(BOOL bServer);
	~CPipe(void);

public:
	HANDLE GetPipe();
	LPCWSTR GetPipeName();
	BOOL CreatePipe(LPCWSTR lpwsName, UINT uMaxInstances = PIPE_UNLIMITED_INSTANCES);
	BOOL DestoryPipe();

	BOOL ConnectNamedPipe(BOOL bConnect = TRUE);

	BOOL WaitNamedPipe(int nTimeout = 0);
	BOOL ReadData(CPipeStream& buf);
	BOOL WriteData(CPipeStream& buf);

	BOOL SetNamedPipeHandleState(DWORD dwMode);

protected:
	BOOL CreateServerPipe(LPCWSTR lpwsName);
	BOOL CreateClientPipe(LPCWSTR lpwsName);

private:
	CString m_strName;
	BOOL m_bServer;
	HANDLE m_hPipe;
	UINT m_uMaxInstance;
};

