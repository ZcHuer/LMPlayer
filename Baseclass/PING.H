#pragma  once

class CPing
{
public:
	void Ping(UINT nRetries,LPCTSTR pstrHost,HWND hWnd,UINT nMsgID);
	int WaitForEchoReply(SOCKET s);
	//ICMP回应的请求和回答函数
	int	SendEchoRequest(SOCKET, LPSOCKADDR_IN);
	DWORD RecvEchoReply(SOCKET, LPSOCKADDR_IN,u_char *pTTL);
	u_short in_cksum(u_short *addr, int len);

protected:
	HWND m_hWnd; //窗口句柄
	UINT m_nMsgID;
};