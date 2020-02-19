
#include "stdafx.h"
#include "ping.h"


void CPing::Ping(UINT nRetries,LPCTSTR pstrHost,HWND hWnd,UINT nMsgID)
{
	m_hWnd = hWnd;
	m_nMsgID=nMsgID;

	CString strCmd;
	strCmd.Format(_T("ping %s -n 1"),pstrHost);

	for (UINT nLoop = 0; nLoop < nRetries; nLoop++)
	{
		if(!::IsWindowVisible(m_hWnd))
			break;

		SECURITY_ATTRIBUTES sa;
		HANDLE hRead,hWrite;
		sa.nLength = sizeof(SECURITY_ATTRIBUTES);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		if (!CreatePipe(&hRead,&hWrite,&sa,0)) {
			//发送报错信息
			if(::IsWindow(m_hWnd))
				PostMessage(m_hWnd,m_nMsgID,0,-1);
			break;
		}
		STARTUPINFO si;
		PROCESS_INFORMATION pi;
		si.cb = sizeof(STARTUPINFO);
		GetStartupInfo(&si);
		si.hStdError = hWrite;
		si.hStdOutput = hWrite;
		si.wShowWindow = SW_HIDE;
		si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

		if (!CreateProcess(NULL,strCmd.GetBuffer(strCmd.GetLength())
			,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi)) {
				//发送报错信息
				if(::IsWindow(m_hWnd))
					PostMessage(m_hWnd,m_nMsgID,0,-1);
				break;
		}
		if(WaitForSingleObject(pi.hProcess, INFINITE)==WAIT_FAILED)
		{
			break;
		}
		CloseHandle(hWrite);

		CStringA strResult="";
		char buffer[4096] ={0} ;
		DWORD bytesRead;
		while(1)
		{
			if(ReadFile(hRead,buffer,4096,&bytesRead,NULL) != NULL)
			{
				strResult += buffer;
			}
			else
			{
				break;
			}
		}
		CloseHandle(hRead);

		CString strResultW=CA2W(strResult);
		if(::IsWindowVisible(m_hWnd))
		{
			int ttlPos = strResultW.Find(L"TTL=",0);
			int nTime=-1;
			if(ttlPos!=-1)
			{
				CString sTime=strResultW.Left(ttlPos);
				int nPos1=sTime.ReverseFind('=');
				int nPos2=sTime.Find(L"ms");
				if(nPos1!=-1 && nPos1!=-1)
				{
					CString s=strResultW.Mid(nPos1+1,nPos2-nPos1-1);
					nTime=_ttoi(s);
				}
			}

			PostMessage(m_hWnd,m_nMsgID,1,(LPARAM) nTime);
			Sleep(500);
		}
	}
	if(::IsWindowVisible(m_hWnd))
		PostMessage(m_hWnd, m_nMsgID, 2, 0);
}