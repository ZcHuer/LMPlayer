#ifndef OSVersion_h__
#define OSVersion_h__

class COSVersion
{
public:
	static COSVersion* GetInstance()
	{
		static COSVersion version;
		return &version;
	}

	BOOL isWindow7OrLater()
	{
		if( m_osVer.dwMajorVersion >= 6 )
		{
			return TRUE;
		}
		return FALSE;
	}

	BOOL IsWindowsXP()
	{
		if (m_osVer.dwMajorVersion == 5 && m_osVer.dwMinorVersion == 1)
		{
			return TRUE;
		}
		return FALSE;
	}

private:
	COSVersion()
	{
		m_osVer.dwOSVersionInfoSize=sizeof(OSVERSIONINFOEX);
		GetVersionEx((OSVERSIONINFO*)&m_osVer);
	}

	~COSVersion() {};

private:
	OSVERSIONINFOEX m_osVer;
};

#endif // OSVersion_h__
