#include "stdafx.h"
#include "FeedbackDlg.h"
#include "../SDK/http/Http.h"
#include "MainDlg.h"
#include "base641.h"


extern _DoMain g_doMain;

/*extern */wstring g_CustomerUrl = L"";
/*extern */wstring g_QQUrl = L"";

#define SUCESS_TIMER 1000
CFeedbackDlg::CFeedbackDlg() : SHostWnd(_T("LAYOUT:XML_FEEDBACK"))
{
	m_hWndParent = NULL;
	m_bSure = FALSE;	
	m_aesEncry.InitKey("2323456765456545", "2323456765456545");
}

CFeedbackDlg::~CFeedbackDlg()
{
}

void CFeedbackDlg::OnQQLink(int nID)
{
	
}
void CFeedbackDlg::OnFeedConcel()
{
	::PostMessage(m_hWnd, WM_QUIT, 0, 0);
}

void CFeedbackDlg::OnFeed()
{
	//意见反馈
	string sUrl = g_doMain.bssDomain;
	wstring wurl =LeTools::s2ws(sUrl).c_str();

	SStringT data = GetCheckProblem();
	wstring advise = _T("");
	SWindow *pwnd = FindChildByName(_T("FeedInfoEdit"));
	if (pwnd)
	{
		SStringT data = pwnd->GetWindowTextW();
		advise +=LeTools::UrlEncode(data.GetBuffer(data.GetLength()));
	}
	wstring contactWay = _T("");
	pwnd = FindChildByName(_T("FeedInfoEditPhone"));
	if (pwnd)
	{
		SStringT data = pwnd->GetWindowTextW();
		contactWay +=LeTools::UrlEncode(data.GetBuffer(data.GetLength()));
	}
	if (contactWay.length()==0 && advise.length()==0 && data.GetLength() ==0)
	{
		return;
	}

	// 拼json串
	Json::Value jvData_Source;
	jvData_Source["problem"] =LeTools::ws2s(data.GetBuffer(data.GetLength()));
	jvData_Source["advise"] =LeTools::ws2s(advise);
	jvData_Source["contactWay"] =LeTools::ws2s(contactWay);
	jvData_Source["source"] = "pc-client";
	Json::FastWriter writer;
	writer.omitEndingLineFeed();
	string sJson = writer.write(jvData_Source);
	LeTools::trim(sJson);
	sJson =LeTools::Gbk2Utf8(sJson);
	string sEncrypt = m_aesEncry.encrypt(sJson);
	DWORD dwSize = sEncrypt.length() * 2;
	unsigned char* cDes = (unsigned char*)malloc(dwSize);
	memset(cDes, 0, dwSize);
	base64_encode((unsigned char*)sEncrypt.c_str(), sEncrypt.length(), cDes, &dwSize);
	sJson = (char*)cDes;
	free(cDes);
	// 加密		
	string signKey = RSA_SIGNKEY;
	Json::Value jvData_Des;
	jvData_Des["signStr"] = sJson;
	jvData_Des["signKey"] = signKey;
	string sJson_Des = writer.write(jvData_Des);

	string sResult =LeTools::doPost(wurl, L"/saveFeedback", sJson_Des);
	sResult =LeTools::ConvertUtf8ToGBK(sResult);
	Json::Reader jr;
	Json::Value jvData;
	if (jr.parse(sResult, jvData))
	{
		if (jvData["code"].asInt() == 0)
		{
			SWindow *pbtn1 = FindChildByName(_T("btn_Feed"));
			SWindow *pbtn2 = FindChildByName(_T("btn_FeedConcel"));
			SWindow *pbtn3 = FindChildByName(_T("MsgSec"));
			pbtn3->SetAttribute(_T("show"), _T("1"));
			pbtn1->SetAttribute(_T("enable"), _T("0"));
			pbtn2->SetAttribute(_T("enable"), _T("0"));
			SetTimer(SUCESS_TIMER, 3000);
		}
	}
	else
	{
		//ReportLog(data);
	}
}
void CFeedbackDlg::ReLoadInfor()
{
	//请求后台配置信息
	string sUrl = g_doMain.bssDomain;
	sUrl.append("/feedback/way?source=pc-client");

	wstring wurl =LeTools::s2ws(sUrl).c_str();
	HttpRequest req(wurl.c_str(), NULL, NULL);
	BOOL bSucc = req.doGet();
	if (bSucc)
	{
		string sResult = req.getStream().str();
		sResult =LeTools::ConvertUtf8ToGBK(sResult);
		Json::Reader jr;
		Json::Value jvData;
		if (jr.parse(sResult, jvData)) {
			m_qqVector.clear();
			if (jvData["code"].asInt() == 0)
			{
				//m_QQCode = jvData["qq"][0]["number"].asCString();
				//m_QQUrl = jvData["qq"][0]["url"].asCString();
				//m_QQName = jvData["qq"][0]["name"].asCString();
				Json::Value jvDataQQ = jvData["qq"];
				int nSize = jvDataQQ.size();
				for (int i =0;i<nSize;i++)
				{
					sFeedBackInfo fInfo;
					fInfo.m_QQCode = jvDataQQ[i]["number"].asCString();
					fInfo.m_QQUrl = jvDataQQ[i]["url"].asCString();
					fInfo.m_QQName = jvDataQQ[i]["name"].asCString();
					m_qqVector.push_back(fInfo);
				}
				m_CustomPhone = jvData["phone"].asCString();
			}
			else
			{
				//m_QQUrl = "https://jq.qq.com/?_wv=1027&k=5dOWLlm";
				//m_QQName = "客户反馈QQ群";
				//m_QQCode = "784929683";

				sFeedBackInfo fInfo;
				fInfo.m_QQCode = "784929683";
				fInfo.m_QQUrl = "https://jq.qq.com/?_wv=1027&k=5dOWLlm";
				fInfo.m_QQName = "客户反馈QQ群";
				m_qqVector.push_back(fInfo);
				m_CustomPhone = "010-86888666,400-833-5757-767";
			}
		}
	
	}


}

void CFeedbackDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == SUCESS_TIMER)
	{
		
		::PostMessage(m_hWnd, WM_QUIT, 0, 0);
		KillTimer(SUCESS_TIMER);
	}

}
void CFeedbackDlg::OnLnkCustomer()
{
	//ShellExecute(NULL, L"open", L"iexplore.exe", g_CustomerUrl.c_str(), 0, SW_SHOW);
}
void CFeedbackDlg::OnLnkQQ()
{
	//wstring wqqUrl =LeTools::s2ws(m_QQUrl);
	//ShellExecute(NULL, L"open", L"iexplore.exe", wqqUrl.c_str(), 0, SW_SHOW);
}
SStringT CFeedbackDlg::GetCheckProblem()
{
	SStringT strRes = _T("");
	for (int i = 1; i < 10; i++)
	{
		wchar_t tagName[5] = {0};
		wsprintfW(tagName, L"ch%d", i);
		SWindow* pWnd = FindChildByName(tagName);
		if (pWnd &&pWnd->IsChecked())
		{
			strRes += pWnd->GetWindowTextW();
			strRes += _T(";");
		}

	}
	return strRes;
}

BOOL CFeedbackDlg::DoModel()
{
	BOOL bEnableParent = FALSE;
	if (!m_hWndParent) {
		m_hWndParent = ::GetActiveWindow();
	}
	m_hWndParent = CMainDlg::g_hMainWnd;
	if (m_hWndParent && m_hWndParent != ::GetDesktopWindow() && ::IsWindowEnabled(m_hWndParent)) {
		::EnableWindow(m_hWndParent, FALSE);
		bEnableParent = TRUE;
	}
	
	if (!Create(m_hWndParent, WS_POPUP | WS_CLIPCHILDREN, 0, 0, 0, 0, 0)) {
		return 0;
	}

	CSimpleWnd::SendMessage(WM_INITDIALOG, (WPARAM)m_hWnd);

	HWND hWndLastActive = ::SetActiveWindow(m_hWnd);

	if (GetExStyle()&WS_EX_TOOLWINDOW)
		::ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
	else
		::ShowWindow(m_hWnd, SW_SHOWNORMAL);

	SetWindowPos(HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// From MFC
	// hide the window before enabling the parent, etc.

	//onPasswordFocusChanged(false);

	if (IsWindow())
	{
		CSimpleWnd::SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}

	if (bEnableParent)
		::EnableWindow(m_hWndParent, TRUE);

	::SetActiveWindow(m_hWndParent);

	if (IsWindow())
		CSimpleWnd::DestroyWindow();

	return 0;
}


void CFeedbackDlg::OnClose()
{
	PostMessage(WM_QUIT);
}

int CFeedbackDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	SetMsgHandled(false);
	return 0;
}

BOOL CFeedbackDlg::OnInitDialog(HWND wndFocus, LPARAM lInitParam)
{
	ReLoadInfor();
	
	/*SWindow *pwndQQ = FindChildByName(_T("lnk_QQ"));
	wstring wqqCode =LeTools::s2ws(m_QQCode);
	pwndQQ->SetWindowTextW(wqqCode.c_str());*/
	string sPos = "[0";
	int id = 3000;
	for (vector<sFeedBackInfo>::iterator it = m_qqVector.begin(); it != m_qqVector.end(); it++)
	{
		sFeedBackInfo info = *it;
		if (info.m_QQCode != "")
		{
			TiXmlDocument doc;
			TiXmlElement* elMain = new TiXmlElement("link");
			doc.LinkEndChild(elMain);
			//<link name="lnk_QQ" pos="[0,{0" colorText="#0296ff" font="size:15,underline:0"></link>
			sPos = sPos + ",{0";
			
			string sName = "qq_" +LeTools::Num2Str(id);
			elMain->SetAttribute("id",LeTools::Num2Str(id).c_str());
			elMain->SetAttribute("tip", info.m_QQName.c_str());
			elMain->SetAttribute("href", info.m_QQUrl.c_str());
			TiXmlText* xmlTex = new TiXmlText(info.m_QQCode.c_str());
			elMain->LinkEndChild(xmlTex);

			TiXmlPrinter print;
			doc.Accept(&print);
			const char* lpDoc = print.CStr();

			SStringT sTag =LeTools::s2ws(lpDoc).c_str();
			SWindow* pWnd = FindChildByName(L"lnk_QQ");
			pWnd->CreateChildren(sTag);
			id++;
		}
		sPos = "[1";
	}

	SWindow *pwndCustom = FindChildByName(_T("lnk_CustomerPhone"));
	wstring wPhone =LeTools::s2ws(m_CustomPhone);
	pwndCustom->SetWindowTextW(wPhone.c_str());
	//pwndCustom->SetAttribute(L"show", L"0");

	return 0;
}


int CFeedbackDlg::ReportLog(SStringT ReportData)
{
	return 0;
}
