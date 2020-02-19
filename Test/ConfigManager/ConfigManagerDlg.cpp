
// ConfigManagerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ConfigManager.h"
#include "ConfigManagerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ��ȡ���ذ汾��
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

// CConfigManagerDlg �Ի���



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


// CConfigManagerDlg ��Ϣ�������

BOOL CConfigManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// �����ļ�·��-ֻ��԰�װ·����
	WCHAR wszBuffer[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, wszBuffer, CSIDL_APPDATA, FALSE);
	PathAppend(wszBuffer, L"\\LePlayer\\Config.ini");
	m_cstrCfg = wszBuffer;
	// ��ȡ�¾ɰ汾
	WCHAR wBuff_Cur[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Update", L"CurVer", L"1.2.0.26", wBuff_Cur, 2048, m_cstrCfg.GetString());
	m_cstrVer_Cur = wBuff_Cur;
	WCHAR wBuff_New[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Update", L"NewVer", L"1.2.0.26", wBuff_New, 2048, m_cstrCfg.GetString());
	m_cstrVer_New = wBuff_New;
	// �Ƿ���Ի���
	WCHAR wBuff_DebugUrl[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Debug", L"DebugUrl", L"", wBuff_DebugUrl, 2048, m_cstrCfg.GetString());
	CString cstrTmp = wBuff_DebugUrl;
	if (-1 == cstrTmp.Find(L"-dev"))
		m_nSel = 0;
	else
		m_nSel = 1;
	// �Ƿ�Ԥ������
	WCHAR wBuff_DebugUrlPach[MAX_PATH] = { 0 };
	GetPrivateProfileString(L"Debug", L"DebugUrlPath", L"", wBuff_DebugUrlPach, 2048, m_cstrCfg.GetString());
	cstrTmp = wBuff_DebugUrlPach;
	if (-1 == cstrTmp.Find(L"pre.shtml"))
		m_bPreView = FALSE;
	else
		m_bPreView = TRUE;

	// �Ƿ�رո���
	int nUpdate = GetPrivateProfileInt(L"Update", L"Update", 1, m_cstrCfg.GetString());
	m_bCloseUpdate = nUpdate ? false : true;

	// �Ƿ������Թ���
	int nJsDebug = GetPrivateProfileInt(L"Debug", L"JsDebug", 0, m_cstrCfg.GetString());
	m_bJsDebug = nJsDebug ? true : false;

	UpdateData(false);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CConfigManagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CConfigManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CConfigManagerDlg::OnBnClicked_Save()
{
	UpdateData();

	// �л�����
	if (0 == m_nSel)
		WritePrivateProfileString(L"Debug", L"DebugUrl", NULL, m_cstrCfg.GetString());
	else if (1 == m_nSel)
		WritePrivateProfileString(L"Debug", L"DebugUrl", L"http://leplayer-dev.vgs.lenovo.com.cn/", m_cstrCfg.GetString());

	// �Ƿ�Ԥ��
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
		

	// �Ƿ�ر���������
	if (m_bCloseUpdate)
		WritePrivateProfileString(L"Update", L"Update", L"0", m_cstrCfg.GetString());
	else
		WritePrivateProfileString(L"Update", L"Update", NULL, m_cstrCfg.GetString());

	// �Ƿ���JsDebug
	if (m_bJsDebug)
	{
		WritePrivateProfileString(L"Debug", L"JsDebug", L"1", m_cstrCfg.GetString());
		// ����copy����
		// ƴ��װĿ¼
		WCHAR wszBuffer_Install[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, wszBuffer_Install, CSIDL_APPDATA, FALSE);
		PathAppend(wszBuffer_Install, L"\\LePlayer\\run\\CEF\\devtools_resources.pak");

		// ƴ��ǰĿ¼
		WCHAR wcPath_Cur[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wcPath_Cur, MAX_PATH);
		PathRemoveFileSpecW(wcPath_Cur);
		PathAppendW(wcPath_Cur, L"\\devtools_resources.pak");

		::CopyFile(wcPath_Cur, wszBuffer_Install, TRUE);
		
		// �����ǰ�汾�������汾��ͬ����ͬʱ����һ��
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

	// �����ŷ���
	WCHAR wszBuffer_Service[MAX_PATH] = { 0 };
	SHGetSpecialFolderPath(NULL, wszBuffer_Service, CSIDL_APPDATA, FALSE);
	PathAppend(wszBuffer_Service, L"\\LePlayer\\LePlayerService.exe");
	::ShellExecuteW(NULL, L"open", wszBuffer_Service, L"/install", NULL, SW_SHOW);
	
	return;
}

void CConfigManagerDlg::OnBnClicked_Cancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
