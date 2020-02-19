#pragma once
#include "..\..\baseclass\Base.h"
#include "..\..\baseclass\Bind.h"
#include "..\..\baseclass\RingBuffer.h"


MIDL_INTERFACE("5AF83C16-9298-455E-B990-2FEF894BD6B8")
IBrowserEvent/* : IUnknown*/
{
public:
	STDMETHOD(OnBrowserCreated)(LPCWSTR lpwsFlag)PURE;												// 创建完成，返回flag，可以loadurl
	STDMETHOD(OnLoadEnd)(LPCWSTR lpwsUrl, BOOL bMain)PURE;											// url对应的loadEnd以及js上下文就绪，可以执行js代码
	STDMETHOD(OnLoadError)(LPCWSTR lpwsUrl, BOOL bMain, int nError)PURE;							// url对应的loaderror
	STDMETHOD(OnJSCallCPlus)(LPCWSTR lpwsUrl, LPCWSTR lpwsId, LPCWSTR lpwsContent)PURE;				// JS回调接口
	STDMETHOD(OnLoadHtmlSize)(LPCWSTR lpwsUrl, LONGLONG llSize)PURE;								// loadhtml大小
	STDMETHOD(OnSnapShot)(std::shared_ptr<CRingBuffer> spBuffer, int nWidth, int nHeight)PURE;      // 截图

};

MIDL_INTERFACE("A19FA66F-1971-4C14-9474-DB2C3A42A04B")
ICefStringArray : IUnknown
{
public:
	STDMETHOD(Appdend)(LPCWSTR lpwsString)PURE;
	STDMETHOD(GetCount)(UINT* lpCount)PURE;
	STDMETHOD(GetAt)(UINT nIndex, BSTR* lpString)PURE;
	STDMETHOD(Clear)()PURE;
};

MIDL_INTERFACE("AB5B73AF-122D-4022-AD44-74DE18B5C9AA")
ICefBrowser : public IUnknown
{
public:
	STDMETHOD(LoadURL)(LPCWSTR lpwsUrl, BOOL bIgnore404 = TRUE)PURE;
	STDMETHOD(Reload)(BOOL bIgnoreCache)PURE;
	STDMETHOD(GoBack)()PURE;
	STDMETHOD(GoForward)()PURE;
	STDMETHOD(AdvisEvent)(IBrowserEvent* lpEvent)PURE;
	STDMETHOD(SetShow)(BOOL bShow)PURE;
	STDMETHOD(AdjustRect)(LPCRECT lprc)PURE;
	STDMETHOD(CallJS)(LPCWSTR lpwsJS)PURE;
	STDMETHOD(Destroy)();
	STDMETHOD(Stop)();
	STDMETHOD(LogError)(LPCWSTR lpwsId, LPCWSTR lpwsContent, LPCWSTR lpwsError)PURE;
	STDMETHOD_(HWND, GetBrowserWindow)()PURE;
	STDMETHOD(CallJS2)(ICefStringArray* IstrArray)PURE;
	STDMETHOD(SelectAll)()PURE;
	STDMETHOD(CopySelect)()PURE;
	//STDMETHOD(SnapShot)(bool bStart, int nWidth, int nHeight, int nFreq)PURE;	
	STDMETHOD(CaptureScreen)(LPCWSTR szFilePath)PURE;
	STDMETHOD(Close)()PURE;
};

enum eProcessType 
{
	eBrowserProcess,
	eRendererProcess,
	eZygoteProcess,
	eOtherProcess,
};

typedef void(__stdcall * CrashHandler)();

MIDL_INTERFACE("8478DD2A-BB4C-4FF4-845F-853916101584")
ICefManager : public IUnknown
{
public:
	STDMETHOD(Init)(HINSTANCE hInstance)PURE;
	STDMETHOD(GetProcessType)(eProcessType& eType)PURE;
	STDMETHOD(RunMessageLoop)()PURE;
	STDMETHOD(CreateBrowser)(HWND hWndParent, LPCRECT lprcWnd, LPCWSTR lpwsLogFile, ICefBrowser** ppBrowser, bool bPopWindow = false)PURE;	
	STDMETHOD(QuitMessageLoop)()PURE;
	STDMETHOD(Shutdown)()PURE;
	STDMETHOD(SetCrashCallBack)(CrashHandler callback)PURE;
};

EXTERN_C	const GUID LIBID_ACCefLib;
EXTERN_C	const GUID CLSID_CefManager;
EXTERN_C	const GUID CLSID_CefBrowser;
EXTERN_C	const GUID CLSID_CefStringArray;
