#include "../stdafx.h"
#include "CUpdateHelper.h"
#include "NewVerDlg.h"
#include "UserChoiceDlg.h"
#include "../MainDlg.h"
#include "../include/LeReport.h"

const std::wstring BASE_URL_REPORT = L"https://sus.lex.lenovo.com.cn";


CUpdatehelper* CUpdatehelper::m_pInstance = NULL;
CUpdatehelper* CUpdatehelper::GetInstance()
{
	if (NULL == m_pInstance)
		m_pInstance = new CUpdatehelper();

	return m_pInstance;
}

void CUpdatehelper::Release()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

CUpdatehelper::CUpdatehelper()
{
	m_bLePlayerAutoRun = false;
	m_bUpdateWork = false;
}

CUpdatehelper::~CUpdatehelper()
{
	m_bLePlayerAutoRun = false;
	m_bUpdateWork = false;
}

bool CUpdatehelper::CheckUpdate()
{
	if (m_bUpdateWork)
		return false;

	m_bUpdateWork = true;

	// �жϱ�������
	if (false == LeTools::IsConfigUpdate())
	{
		m_bUpdateWork = false;
		return false;
	}

	// ��ȡ��������
	if (false == GetUpdateInfo())
	{
		m_bUpdateWork = false;
		return false;
	}

	// �Ƚϰ汾�ţ��ж��Ƿ���Ҫ����
	string strVer;
	LeTools::GetVersion(eLmplayer, strVer);
	bool bNeedUpdate = IsNewVersion(m_sUpdateInfo.versionName.c_str(), strVer.c_str());
	
	if (!bNeedUpdate)
	{
		m_bUpdateWork = false;
		return false;
	}

	// �����������ͣ����ݷ���˵����ã��ֱ�������ǿ���������������������ã�ֻ�߾�Ĭ����
	if ("1" == m_sUpdateInfo.updateType || m_bLePlayerAutoRun)// ��Ĭ����
		return SilentInstall(m_sUpdateInfo.appLoadUrl, m_sUpdateInfo.versionName);
	else if ("2" == m_sUpdateInfo.updateType)// ǿ������
		return InstallNow();
	else if ("3" == m_sUpdateInfo.updateType)// �û�ѡ��
		return UserChoiceUpdate();

	m_bUpdateWork = false;
	return false;
}

bool CUpdatehelper::ManualCheck()
{
	if (m_bUpdateWork)
		return false;

	m_bUpdateWork = true;
	// ��ȡ��������
	if (false == GetUpdateInfo())
	{
		m_bUpdateWork = false;
		return false;
	}

	// �Ƚϰ汾�ţ��ж��Ƿ���Ҫ����
	string strVer;
	LeTools::GetVersion(eLmplayer, strVer);
	bool bNeedUpdate = IsNewVersion(m_sUpdateInfo.versionName.c_str(), strVer.c_str());
	if (!bNeedUpdate)
	{
		CNewVerDlg dlg;
		dlg.DoModel();
		m_bUpdateWork = false;
		return true;
	}
	else
	{
		InstallNow();
	}

	m_bUpdateWork = false;
	return false;
}

bool CUpdatehelper::InstallNow()
{
	if (m_sUpdateInfo.appLoadUrl.length() == 0 || m_sUpdateInfo.versionName.length() == 0)
		return false;

	m_bUpdateWork = true;
	SUpdateDlg dlg(m_sUpdateInfo.appLoadUrl, m_sUpdateInfo.versionName);		
	dlg.DoModel();

	// ����������̨ͳ�Ʊ���
	ReportDowsloadResult(dlg.IsSuccessed());
	if (dlg.IsSuccessed())
	{
		// д���°汾
		WriteVersionToCfg_New(m_sUpdateInfo.versionName);
		WriteVersionToCfg_Old("0");

		// �����ݺ�̨����
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "success");

		// ��������س���
		LeTools::KillProcess(_T("LMPlayer.exe"));
		// ����launch
		WCHAR wcMoudlePath[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wcMoudlePath, MAX_PATH);
		PathRemoveFileSpecW(wcMoudlePath);
		PathRemoveFileSpecW(wcMoudlePath);
		wstring wWorkPath = wcMoudlePath;
		PathAppendW(wcMoudlePath, L"\\LMPlayer.exe");

		if (PathFileExistsW(wcMoudlePath))
			::ShellExecuteW(NULL, L"open", wcMoudlePath, L"runnew", wWorkPath.c_str(), SW_SHOW);
	}

	m_bUpdateWork = false;
	return false;
}

bool CUpdatehelper::GetUpdateInfo()
{
	return m_cUpdate.GetUpdateConfig(m_sUpdateInfo);
}

bool CUpdatehelper::IsNewVersion(const char * version_Net, const char * version_Local)
{
	string strversion_Net = version_Net;
	string strversion_Local = version_Local;
	if (strversion_Net.empty())
		return false;
	if (strversion_Local.empty())
		return true;

	size_t pos = 0;
	int number_Net = 0;
	int number_Local = 0;
	string pattern(".");
	int icpt = pattern.size();
	std::vector<string> result_Net;
	std::vector<string> result_Local;
	int iposbf = 0;
	while (pos != string::npos)
	{
		pos = strversion_Net.find(pattern, pos);
		if (pos != string::npos)
		{
			int istart = iposbf;
			if (0 != istart)
				istart = istart + icpt;

			std::string s = strversion_Net.substr(istart, pos - istart);
			result_Net.push_back(s);
			++number_Net;
			if (3 == number_Net)
			{
				s = strversion_Net.substr(pos + 1, strversion_Net.length());
				result_Net.push_back(s);
				break;
			}
			iposbf = pos;
			pos += (pattern.size() + 1);
		}
	}

	if (number_Net != 3 || result_Net.size() != 4)
		return false;

	pos = 0;
	iposbf = 0;
	while (pos != string::npos)
	{
		pos = strversion_Local.find(pattern, pos);
		if (pos != string::npos)
		{
			int istart = iposbf;
			if (0 != istart)
				istart = istart + icpt;

			std::string s = strversion_Local.substr(istart, pos - istart);
			result_Local.push_back(s);
			++number_Local;
			if (3 == number_Local)
			{
				s = strversion_Local.substr(pos + 1, strversion_Local.length());
				result_Local.push_back(s);
				break;
			}
			iposbf = pos;
			pos += (pattern.size() + 1);
		}
	}
	 
	if (number_Local != 3 || result_Local.size() != 4)
		return true;

	std::vector<string>::iterator it_N = result_Net.begin(), it_L = result_Local.begin();
	for (int i = 0; i < 4; i++)
	{
		int nNet = stol((*it_N).c_str());
		int nLocal = stol((*it_L).c_str());
		if (nNet > nLocal)
			return true;
		else if (nNet < nLocal)
			return false;

		it_N++;
		it_L++;
	}

	return false;
}

bool CUpdatehelper::SilentInstall(string strDownLoadUrl, string strSvrVer)
{
	FLOG(L"��Ĭ����-��ʼ");
	m_bUpdateWork = true;
	if (strDownLoadUrl.length() == 0)
	{
		m_bUpdateWork = false;
		return false;
	}

	bool bRet = m_cUpdate.DownloadAndUnzip(strDownLoadUrl.c_str(), strSvrVer.c_str());
	ReportDowsloadResult(bRet);
	if (bRet)
	{
		FLOG(L"��Ĭ����-�ɹ�-�޸İ汾��");
		// д���°汾
		WriteVersionToCfg_New(m_sUpdateInfo.versionName);
		WriteVersionToCfg_Old("0");

		// �����ݺ�̨����
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "success");
	}

	m_bUpdateWork = false;
	return bRet;
}

bool CUpdatehelper::UserChoiceUpdate(bool bUnKnownCP/* = false*/)
{
	m_bUpdateWork = true;
	CUserChoiceDlg dlg("", bUnKnownCP ? "��ǰ�汾���ͣ������������°汾" : "��⵽�°汾�������������°汾", "��������", true);
	dlg.DoModel();
	
	if (dlg.IsBtnOk())
	{
		m_bUpdateWork = false;
		return InstallNow();
	}

	m_bUpdateWork = false;
	return false;
}

void CUpdatehelper::SetLeplayerAutoRun(bool bAutoRun)
{
	if (bAutoRun)
		FLOG(L"����ģ��-��������-��")
	else
		FLOG(L"����ģ��-��������-��")

	m_bLePlayerAutoRun = bAutoRun;
	return;
}

bool CUpdatehelper::IsLeplayerAutoRun()
{
	return m_bLePlayerAutoRun;
}

bool CUpdatehelper::IsUpdateing()
{
	return m_cUpdate.IsDownLoading() && m_bUpdateWork;
}

void CUpdatehelper::CancelDownload()
{
	return m_cUpdate.CancelDownload();
}

void CUpdatehelper::WriteVersionToCfg_New(string strVer)
{
	// ��ȡConfig�ļ�·��
	CHAR cPath[MAX_PATH] = { 0 };
	wchar_t wpath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wpath, MAX_PATH);
	::PathRemoveFileSpecW(wpath);
	PathAppend(wpath, L"\\..\\Config.ini");
	string str =LeTools::ws2s(wpath);
	PathAppendA(cPath, str.c_str());
	WritePrivateProfileStringA("Update", "NewVer", strVer.c_str(), cPath);
	return;
}

void CUpdatehelper::WriteVersionToCfg_Old(string strVer)
{
	// ��ȡConfig�ļ�·��
	CHAR cPath[MAX_PATH] = { 0 };
	wchar_t wpath[MAX_PATH] = { 0 };
	::GetModuleFileNameW(NULL, wpath, MAX_PATH);
	::PathRemoveFileSpecW(wpath);
	PathAppend(wpath, L"\\..\\Config.ini");
	string str =LeTools::ws2s(wpath);
	PathAppendA(cPath, str.c_str());
	WritePrivateProfileStringA("Update", "CurVer", strVer.c_str(), cPath);
	return;
}

void CUpdatehelper::ReportDowsloadResult(bool bRet)
{
	string sData = "appId=";
	sData.append(LeTools::Num2Str(m_sUpdateInfo.id));
	sData.append("&sn=");
	string strMac =LeTools::GetMacString();
	sData.append(strMac);
	if (bRet)
	{
		sData.append("&status=1");
		sData.append("&message=�ɹ�");
	}
	else
	{
		sData.append("&status=0");
		sData.append("&message=ʧ��");
	}

	string sResult =LeTools::doPostUrl(BASE_URL_REPORT, L"/api/save", sData);
	sResult =LeTools::ConvertUtf8ToGBK(sResult);
	return;
}

bool CUpdatehelper::SwitchVerNow()
{
	m_bUpdateWork = true;
	CUserChoiceDlg dlg("", "��Ϊ�����������°汾", "����ʹ��", false);
	dlg.DoModel();

	if (dlg.IsBtnOk())
	{
		m_bUpdateWork = false;

		// ��������س���
		LeTools::KillProcess(_T("LMPlayer.exe"));
		// ����launch
		WCHAR wcMoudlePath[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wcMoudlePath, MAX_PATH);
		PathRemoveFileSpecW(wcMoudlePath);
		PathRemoveFileSpecW(wcMoudlePath);
		wstring wWorkPath = wcMoudlePath;
		PathAppendW(wcMoudlePath, L"\\LMPlayer.exe");

		if (PathFileExistsW(wcMoudlePath))
			::ShellExecuteW(NULL, L"open", wcMoudlePath, L"runnew", wWorkPath.c_str(), SW_SHOW);

		return true;
	}

	m_bUpdateWork = false;
	return false;
}

bool CUpdatehelper::UserChoiceSwitch()
{
	m_bUpdateWork = true;
	CUserChoiceDlg dlg("", "��Ϊ�����������°汾", "����ʹ��", true);
	dlg.DoModel();

	if (dlg.IsBtnOk())
	{
		m_bUpdateWork = false;

		// ��������س���
		LeTools::KillProcess(_T("LMPlayer.exe"));
		// ����launch
		WCHAR wcMoudlePath[MAX_PATH] = { 0 };
		::GetModuleFileNameW(NULL, wcMoudlePath, MAX_PATH);
		PathRemoveFileSpecW(wcMoudlePath);
		PathRemoveFileSpecW(wcMoudlePath);
		wstring wWorkPath = wcMoudlePath;
		PathAppendW(wcMoudlePath, L"\\LMPlayer.exe");

		if (PathFileExistsW(wcMoudlePath))
			::ShellExecuteW(NULL, L"open", wcMoudlePath, L"runnew", wWorkPath.c_str(), SW_SHOW);

		return true;
	}

	m_bUpdateWork = false;
	return false;
}

void CUpdatehelper::WhenLeplayerActivation()
{
	// �����򼤻�󣬼����߿�������δ�ߵ��������򣨿��������������û���
	if (IsLeplayerAutoRun())
	{
		FLOG(L"Leplayer����");
		SetLeplayerAutoRun(false);
		// ��ȡ���е���������
		string strUpdateType = GetUpdateType();
		
		string strVer_New, strVer_Old;
		LeTools::GetVersion(eLmplayer, strVer_New);
		LeTools::GetOldVersion(eLmplayer, strVer_Old);
		// �Ƚ����߰汾�ͱ��ؾɰ汾�ţ��ж��Ƿ���Ҫ����
		if (IsNewVersion(m_sUpdateInfo.versionName.c_str(), strVer_Old.c_str()))
		{
			if ("1" == strUpdateType)
			{// ��Ĭ����
				return;
			}
			else if ("2" == strUpdateType)
			{// ǿ������
				if (0 != strVer_New.compare(strVer_Old.c_str()))
					SwitchVerNow();
				else
					InstallNow();
			}
			else if ("3" == strUpdateType)
			{
				if (0 != strVer_New.compare(strVer_Old.c_str()))
					UserChoiceSwitch();
				else
					UserChoiceUpdate();
			}
		}		
	}
	return;
}

string CUpdatehelper::GetUpdateType()
{
	return m_sUpdateInfo.updateType;
}
