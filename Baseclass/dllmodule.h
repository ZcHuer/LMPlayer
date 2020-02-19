#pragma once

template<bool bFree>
class CDllModuleT
{
public:
	CDllModuleT():m_hDll(NULL){}
	explicit CDllModuleT(HMODULE hDll):m_hDll(hDll){}
	~CDllModuleT() 
	{ 
		if (bFree) 
			Free(); 
	}
#if _MSC_VER >= 1900 //VS2015
	CDllModuleT(const CDllModuleT& ref) = delete;
	CDllModuleT(CDllModuleT&& rval) = delete;
	CDllModuleT& operator=(const CDllModuleT& ref) = delete;
	CDllModuleT& operator=(CDllModuleT&& rval) = delete;
#else //#if _MSC_VER >= 1900 //VS2015
	CDllModuleT(const CDllModuleT& ref);
	CDllModuleT& operator=(const CDllModuleT& ref);
#endif //#if _MSC_VER >= 1900 //VS2015
	operator HMODULE() { return m_hDll; }

public:
	/*
	加载DLL，
	如果为相对路径，则根据所在exe文件路径计算绝对路径并加载
	*/
	HRESULT Load(LPCWSTR lpwsDllFile)
	{
		if(NULL == lpwsDllFile)
			return E_INVALIDARG;

		WCHAR wszExePath[MAX_PATH] = {0};
		if(PathIsRelative(lpwsDllFile))
		{			
			GetModuleFileName(NULL, wszExePath, MAX_PATH);
			PathRemoveFileSpec(wszExePath);
			PathAppend(wszExePath, lpwsDllFile);
			lpwsDllFile = wszExePath;
		}

		if(!PathFileExists(lpwsDllFile))
			return E_FAIL;

		return LoadLibrary(lpwsDllFile);
	}

	/*
	加载DLL，可以指定加载方式见LoadLibraryEx， 
	如果为相对路径，则根据所在exe文件路径计算绝对路径并加载
	*/
	HRESULT LoadEx(LPCWSTR lpwsDllFile, DWORD dwFlag)
	{
		if (NULL == lpwsDllFile)
			return E_INVALIDARG;

		WCHAR wszExePath[MAX_PATH] = { 0 };
		if (PathIsRelative(lpwsDllFile))
		{
			GetModuleFileName(NULL, wszExePath, MAX_PATH);
			PathRemoveFileSpec(wszExePath);
			PathAppend(wszExePath, lpwsDllFile);
			lpwsDllFile = wszExePath;
		}

		if (!PathFileExists(lpwsDllFile))
			return E_FAIL;

		return LoadLibraryEx(lpwsDllFile, dwFlag);
	}

	/*
	加载DLL，直接调用LoadLibrary（）
	*/
	HRESULT LoadLibrary(LPCWSTR lpwsDll)
	{
		if (NULL == lpwsDll)
			return E_INVALIDARG;

		if (m_hDll)
			return S_FALSE;

		m_hDll = ::LoadLibrary(lpwsDll);
		return m_hDll ? S_OK : E_FAIL;
	}

	/*
	加载DLL，直接调用LoadLibraryEx
	*/
	HRESULT LoadLibraryEx(LPCWSTR lpwsDll, DWORD dwFlag)
	{
		if (NULL == lpwsDll)
		return E_INVALIDARG;

		if (m_hDll)
			return S_FALSE;

		m_hDll = ::LoadLibraryEx(lpwsDll, NULL, dwFlag);
		return m_hDll ? S_OK : E_FAIL;
	}

	/*
	卸载DLL， 直接调用FreeLibrary
	*/
	void Free()
	{
		if(m_hDll)
		{
			FreeLibrary(m_hDll);
			m_hDll = NULL;
		}
	}

	/*
	挂载DLL， 返回之前的Dll
	*/
	HMODULE Attach(HMODULE hDll)
	{
		HMODULE hRtn = m_hDll;
		m_hDll = hDll;
		return hRtn;
	}

	/*
	取消挂载Dll, 返回当前的Dll
	*/
	HMODULE Detach()
	{
		HMODULE hRtn = m_hDll;
		m_hDll = NULL;
		return hRtn;
	}
	
	/*
	函数模版
	根据函数定义，从Dll中获取导出函数地址，并转换为函数指针
	例：
	typedef HRESULT (__stdcall * PFNDllGetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
	PFNDllGetClassObject pfn = dll.GetFunc<PFNDllGetClassObject>("DllGetClassObject");
	*/
	template<class F>
	F GetFunc(LPCSTR lpsFuncName)
	{
		if(NULL == m_hDll || NULL == lpsFuncName)
			return (F)NULL;
		return (F)GetProcAddress(m_hDll, lpsFuncName);
	}

protected:
	HMODULE m_hDll;
};

typedef CDllModuleT<true> CDllModule;			//析构函数自动释放dll
typedef CDllModuleT<false> CDllHandle;			//析构函数不释放dll

typedef HRESULT(__stdcall * PFNDllGetClassObject)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);

/*
函数模版
调用导出函数DllGetClassObject（）获取进程内COM对象
*/
template<class I>
HRESULT DllCreateObject(HMODULE hDll, REFCLSID refclsid, I** lppObj)
{
	if(NULL == hDll || NULL == lppObj)
		return E_INVALIDARG;

	CDllHandle dll(hDll);
	PFNDllGetClassObject pfn = dll.GetFunc<PFNDllGetClassObject>("DllGetClassObject");
	dll.Detach();
	if(NULL == pfn)
		return E_FAIL;

	IClassFactory* pFC = NULL;
	HRESULT hr = pfn(refclsid, __uuidof(IClassFactory), (void**)&pFC);
	if(SUCCEEDED(hr))
	{
		hr = pFC->CreateInstance(NULL, __uuidof(I), (void**)lppObj);
		pFC->Release();
	}
	return hr;
}
