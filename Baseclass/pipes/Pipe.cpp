#include "stdafx.h"
#include "Pipe.h"

CPipe::CPipe(BOOL bServer) : m_hPipe(INVALID_HANDLE_VALUE), m_bServer(bServer), m_uMaxInstance(PIPE_UNLIMITED_INSTANCES)
{

}

CPipe::~CPipe(void)
{
	DestoryPipe();
}

HANDLE CPipe::GetPipe()
{
	return m_hPipe;
}

LPCWSTR CPipe::GetPipeName()
{
	return m_strName;
}

BOOL CPipe::CreatePipe(LPCWSTR lpwsName, UINT uMaxInstances/* = PIPE_UNLIMITED_INSTANCES*/)
{
	if(NULL == lpwsName)
		return FALSE;

	if(INVALID_HANDLE_VALUE != m_hPipe)
		return FALSE;
	
	m_strName = lpwsName;
	m_uMaxInstance = uMaxInstances;
	
	BOOL bRtn = m_bServer ? CreateServerPipe(lpwsName) : CreateClientPipe(lpwsName);
	
	return bRtn;
}

BOOL CPipe::DestoryPipe()
{
	if(INVALID_HANDLE_VALUE != m_hPipe)
	{
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}
	return TRUE;
}

BOOL CPipe::WaitNamedPipe(int nTimeout/* = 0*/)
{
	if(m_strName.IsEmpty())
		return FALSE;

	if(nTimeout == 0)
		nTimeout = NMPWAIT_USE_DEFAULT_WAIT;
	else if(nTimeout == INFINITE)
		nTimeout = NMPWAIT_WAIT_FOREVER;

	BOOL bRtn = ::WaitNamedPipe(m_strName, nTimeout);
	return bRtn;
}

BOOL CPipe::CreateServerPipe(LPCWSTR lpwsName)
{
	if(INVALID_HANDLE_VALUE != m_hPipe)
		return FALSE;

	SECURITY_DESCRIPTOR desc;
	::InitializeSecurityDescriptor( &desc, SECURITY_DESCRIPTOR_REVISION);
	::SetSecurityDescriptorDacl( &desc, TRUE, NULL, FALSE );     // Allowing all access to the object 

	SECURITY_ATTRIBUTES securityAttributes;
	securityAttributes.lpSecurityDescriptor = &desc;
	securityAttributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	securityAttributes.bInheritHandle = 1; 

	//Create NamedPip
	m_hPipe = CreateNamedPipe( lpwsName,	// pipe name
		(m_uMaxInstance == 1 ? 
		FILE_FLAG_FIRST_PIPE_INSTANCE
		: 0) |
		PIPE_ACCESS_INBOUND
		|PIPE_ACCESS_OUTBOUND,			// read-only access
		PIPE_TYPE_MESSAGE |				// message-type pipe
		PIPE_READMODE_MESSAGE|			// message read mode
		PIPE_WAIT,						// blocking mode
		m_uMaxInstance,					// unlimited instances
		0,								// output buffer size
		0,								// input buffer size
		20000,							// client time-out
		&securityAttributes );			// no security attributes

	if(INVALID_HANDLE_VALUE == m_hPipe)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}
	return TRUE;
}

BOOL CPipe::CreateClientPipe(LPCWSTR lpwsName)
{
	if(NULL == lpwsName || INVALID_HANDLE_VALUE != m_hPipe)
		return FALSE;
	
	m_hPipe = CreateFile(lpwsName, GENERIC_WRITE|GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	return (INVALID_HANDLE_VALUE != m_hPipe);
}

BOOL CPipe::ConnectNamedPipe(BOOL bConnect/* = TRUE*/)
{
	if(INVALID_HANDLE_VALUE == m_hPipe || !m_bServer)
		return FALSE;

	if(bConnect)
		return ::ConnectNamedPipe(m_hPipe, NULL);

	return ::DisconnectNamedPipe(m_hPipe);
}

BOOL CPipe::ReadData(CPipeStream& buf)
{
	if(INVALID_HANDLE_VALUE == m_hPipe)
		return FALSE;

	if(NULL == buf.CreateStream(DEF_PIPESTREAM_BUFFERSIZE))
		return FALSE;

	UINT uSkip = 0;
	BOOL bReadSecond = FALSE;
	do 
	{
		DWORD dwReaded = 0;
		if(!ReadFile(m_hPipe, buf.GetStream() + uSkip, buf.GetStreamSize() - uSkip, &dwReaded, NULL))
		{
			if(ERROR_MORE_DATA == GetLastError())
			{
				DWORD dwSize = GetFileSize(m_hPipe, NULL);
				if (dwSize != INVALID_FILE_SIZE)
				{
					dwSize = (((dwSize + 0xF) >> 4) << 4); //按16字节对齐

														   //备份原始数据
					LPBYTE pBKBuf = buf.Detach();
					UINT uBKSize = (UINT)dwReaded;

					if (NULL == buf.CreateStream(uBKSize + dwSize))
					{
						ATLASSERT(FALSE);
						return FALSE;
					}

					//还原原始数据
					memcpy(buf.GetStream(), pBKBuf, uBKSize);

					//释放内存
					CPipeStream bufTemp; bufTemp.Attach(pBKBuf, uBKSize);

					//读取数据
					uSkip = uBKSize;
					bReadSecond = TRUE;
				}
				continue;
			}
			else
			{
				return FALSE;
			}
		}
		if(bReadSecond)
			break;
	} while(bReadSecond);

	return TRUE;
}

BOOL CPipe::WriteData(CPipeStream& buf)
{
	if(INVALID_HANDLE_VALUE == m_hPipe)
		return FALSE;

	if(buf.GetStreamSize() <= 0)
		return FALSE;

	DWORD dwWrited = 0;
	if(!WriteFile(m_hPipe, buf.GetStream(), buf.GetStreamSize(), &dwWrited, NULL))
	{
		return FALSE;
	}
	::FlushFileBuffers(m_hPipe);
	return TRUE;
}

BOOL CPipe::SetNamedPipeHandleState(DWORD dwMode)
{
	if(INVALID_HANDLE_VALUE == m_hPipe)
	{
		ATLASSERT(FALSE);
		return FALSE;
	}

	return ::SetNamedPipeHandleState(m_hPipe, &dwMode, NULL, NULL);
}

//=================================================================================================
