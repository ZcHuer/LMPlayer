
// ConfigManagerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ConfigManager.h"
#include "ConfigManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 获取本地版本号
void GetVersion(CString& cstr)
{
	CString cstrCfg;
	WCHAR wszBuffer[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, wszBuffer, CSIDL_APPDATA, FALSE);

// 	string tmp = szBuffer;
// 	tmp += "\\Lenovo\\lebo";
// 
// 	GlobalConfig::GetConfigFilePath(wstrCfg);
// 	WCHAR wBuff[MAX_PATH] = { 0 };
// 	GetPrivateProfileString(L"Update", L"NewVer", CURVER, wBuff, 2048, wstrCfg.c_str());
// 	wstr = wBuff;
// 	if (wstr.length() == 0)
// 		wstr = CURVER;
	return;
}
// 
// static void GetOldVersion(wstring& wstr)
// {
// 	wstring wstrCfg;
// 	GlobalConfig::GetConfigFilePath(wstrCfg);
// 	WCHAR wBuff[MAX_PATH] = { 0 };
// 	GetPrivateProfileString(L"Update", L"CurVer", CURVER, wBuff, 2048, wstrCfg.c_str());
// 	wstr = wBuff;
// 	if (wstr.length() == 0)
// 		wstr = CURVER;
// 	return;
// }

// CConfigManagerDlg 对话框



CConfigManagerDlg::CConfigManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CONFIGMANAGER_DIALOG, pParent)	
	, m_bPreView(FALSE)
	, m_bCloseUpdate(FALSE)
	, m_bJsDebug(FALSE)
	, m_cstrVer_Cur(_T(""))
	, m_cstrVer_New(_T(""))
	, m_nSel(0)
	, m_cstrCfg(L"")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CConfigManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_PRE, m_bPreView);
	DDX_Check(pDX, IDC_CHECK_UPDATE, m_bCloseUpdate);
	DDX_Check(pDX, IDC_CHECK_JSDEBUG, m_bJsDebug);
	DDX_Text(pDX, IDC_EDIT_CURVER, m_cstrVer_Cur);
	DDX_Text(pDX, IDC_EDIT_NEWVER, m_cstrVer_New);
	DDX_Radio(pDX, IDC_RADIO_NOMAL, m_nSel);
}

BEGIN_MESSAGE_MAP(CConfigManagerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL, &CConfigManagerDlg::OnBnClicked_Save)
	ON_BN_CLICKED(IDOK, &CConfigManagerDlg::OnBnClicked_Cancel)
	ON_BN_CLICKED(IDC_RADIO_NOMAL, &CConfigManagerDlg::OnBnClickedRadioNomal)
	ON_BN_CLICKED(IDC_RADIO_TEST, &CConfigManagerDlg::OnBnClickedRadioTest)
END_MESSAGE_MAP()


// CConfigManagerDlg 消息处理程序

BOOL CConfigManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// 配置文件路径-只针对安装路径下
	WCHAR wszBuffer[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, wszBuffer, CSIDL_APPDATA, FALSE);
	PathAppend(wszBuffer, L"\\LePlayer\\Config.ini");
	m_cstrCfg = wszBuffer;
	// 获取新旧版本
	WCHAR wBuff_Cur[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Update", L"CurVer", L"1.2.0.26", wBuff_Cur, 2048, m_cstrCfg.GetString());
	m_cstrVer_Cur = wBuff_Cur;
	WCHAR wBuff_New[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Update", L"NewVer", L"1.2.0.26", wBuff_New, 2048, m_cstrCfg.GetString());
	m_cstrVer_New = wBuff_New;
	// 是否测试环境
	WCHAR wBuff_DebugUrl[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Debug", L"DebugUrl", L"", wBuff_DebugUrl, 2048, m_cstrCfg.GetString());
	CString cstrTmp = wBuff_DebugUrl;
	if (-1 == cstrTmp.Find(L"-dev"))
		m_nSel = 0;
	else
		m_nSel = 1;
	// 是否预览环境
	WCHAR wBuff_DebugUrlPach[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Debug", L"DebugUrlPath", L"", wBuff_DebugUrlPach, 2048, m_cstrCfg.GetString());
	cstrTmp = wBuff_DebugUrlPach;
	if (-1 == cstrTmp.Find(L"pre.shtml"))
		m_bPreView = FALSE;
	else
		m_bPreView = TRUE;

	// 是否关闭更新
	int nUpdate = GetPrivateProfileInt(L"Update", L"Update", 1, m_cstrCfg.GetString());
	m_bCloseUpdate = nUpdate ? false : true;

	// 是否开启调试工具
	int nJsDebug = GetPrivateProfileInt(L"Debug", L"JsDebug", 0, m_cstrCfg.GetString());
	m_bJsDebug = nJsDebug ? true : false;

	UpdateData(false);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CConfigManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CConfigManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConfigManagerDlg::OnBnClicked_Save()
{
	UpdateData();

	// 切换环境
	if (0 == m_nSel)
		WritePrivateProfileString(L"Debug", L"DebugUrl", NULL, m_cstrCfg.GetString());
	else if (1 == m_nSel)
		WritePrivateProfileString(L"Debug", L"DebugUrl", L"http://leplayer-dev.vgs.lenovo.com.cn/", m_cstrCfg.GetString());

	// 是否预览
	if (m_bPreView)
	{
		WritePrivateProfileString(L"Debug", L"DebugUrlPath", L"back/urls/leplayer/pre.shtml", m_cstrCfg.GetString());
		WritePrivateProfileString(L"Debug", L"JsPre", L"1", m_cstrCfg.GetString());
	}
		
	else
	{
		WritePrivateProfileString(L"Debug", L"DebugUrlPath", NULL, m_cstrCfg.GetString());
		WritePrivateProfileString(L"Debug", L"JsPre", NULL, m_cstrCfg.GetString());
	}
		

	// 是否关闭升级功能
	if (m_bCloseUpdate)
		WritePrivateProfileString(L"Update", L"Update", L"0", m_cstrCfg.GetString());
	else
		WritePrivateProfileString(L"Update", L"Update", NULL, m_cstrCfg.GetString());

	// 是否开启JsDebug
	if (m_bJsDebug)
	{
		WritePrivateProfileString(L"Debug", L"JsDebug", L"1", m_cstrCfg.GetString());
		// 增加copy动作
		// 拼安装目录
		WCHAR wszBuffer_Install[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, wszBuffer_Install, CSIDL_APPDATA, FALSE);
		PathAppend(wszBuffer_Install, L"\\LePlayer\\run\\CEF\\devtools_resources.pak");

		// 拼当前目录
		WCHAR wcPath_Cur[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wcPath_Cur, MAX_PATH);
		PathRemoveFileSpecW(wcPath_Cur);
		PathAppendW(wcPath_Cur, L"\\devtools_resources.pak");

		::CopyFile(wcPath_Cur, wszBuffer_Install, TRUE);
		
		// 如果当前版本跟就绪版本不同，则同时复制一份
		if (0 != m_cstrVer_New.CompareNoCase(m_cstrVer_Cur))
		{
			WCHAR wszBuffer_New[MAX_PATH] = { 0 };
			SHGetSpecialFolderPath(NULL, wszBuffer_New, CSIDL_APPDATA, FALSE);
			PathAppend(wszBuffer_New, L"\\LePlayer");
			PathAppend(wszBuffer_New, m_cstrVer_New.GetString());
			PathAppend(wszBuffer_New, L"\\CEF\\devtools_resources.pak");
			::CopyFile(wcPath_Cur, wszBuffer_New, TRUE);
		}
	}		
	else
		WritePrivateProfileString(L"Debug", L"JsDebug", NULL, m_cstrCfg.GetString());

	// 重启伺服器
	WCHAR wszBuffer_Service[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, wszBuffer_Service, CSIDL_APPDATA, FALSE);
	PathAppend(wszBuffer_Service, L"\\LePlayer\\LePlayerService.exe");
	::ShellExecuteW(NULL, L"open", wszBuffer_Service, L"/install", NULL, SW_SHOW);
	
	return;
}

void CConfigManagerDlg::OnBnClicked_Cancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

void CConfigManagerDlg::OnBnClickedRadioNomal()
{
	m_nSel = 0;
	return;
}

void CConfigManagerDlg::OnBnClickedRadioTest()
{
	m_nSel = 1;
	return;
}
