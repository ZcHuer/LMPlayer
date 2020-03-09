#include "CMDParser.h"
#include "../include/LeTools.h"
#include "../SDK/Flog/FileLog.h"
#include "../include/json/json.h"
#include "LeReport.h"
#include "Data_RealTime.h"

CCMDParser::CCMDParser()
{
	m_bRun = false;
	m_bPlay = false;
	m_bFunc = false;
	m_bUrl = false;
	m_wstrCmd_Run = L"";
	m_wstrCmd_Play = L"";
	m_wstrCmd_Func = L"";
	m_wstrCmd_Url = L"";
}

CCMDParser::~CCMDParser()
{
}

void CCMDParser::Parse(wstring wstrCmd)
{
	FLOG(L"CCMDParser::Parse begin");
	// Ĭ��������ָ��
	m_bRun = true;
	// ����ԭ�е�ָ��
	ParseOldCmd(wstrCmd);
	// �����淶�ĸ�ʽ��ָ�4�����Ϳ��Բ���
	wstring wstrTmp = wstrCmd;
	vector<wstring> vec_wstr;	
	int nPos = wstrTmp.find(L" ");
	while (nPos > 0)
	{
		wstring wstrTmp_L = wstrTmp.substr(0, nPos);
		vec_wstr.push_back(wstrTmp_L);
		wstrTmp = wstrTmp.substr(nPos + 1, wstrTmp.length() - nPos - 1);
		nPos = wstrTmp.find(L" ");
	}
	if (wstrTmp.length() > 0)
		vec_wstr.push_back(wstrTmp);

	for (vector<wstring>::iterator it = vec_wstr.begin(); it != vec_wstr.end(); it++)
	{
		wstring wstrTmp = *it;
		int nPos = wstrTmp.find(L"lv_r:");
		if (-1 != nPos)
		{
			m_bRun = true;
			m_wstrCmd_Run = wstrTmp.substr(nPos + 5, wstrTmp.length() - nPos - 5);
		}
		nPos = wstrTmp.find(L"lv_p:");
		if (-1 != nPos)
		{
			m_bPlay = true;
			m_wstrCmd_Play = wstrTmp.substr(nPos + 5, wstrTmp.length() - nPos - 5);
			m_wstrCmd_Play = LeTools::replace(m_wstrCmd_Play, L"\\", L"");
		}
		nPos = wstrTmp.find(L"lv_f:");
		if (-1 != nPos)
		{
			m_bFunc = true;
			m_wstrCmd_Func = wstrTmp.substr(nPos + 5, wstrTmp.length() - nPos - 5);
		}
		nPos = wstrTmp.find(L"lv_u:");
		if (-1 != nPos)
		{
			m_bUrl = true;
			m_wstrCmd_Url = wstrTmp.substr(nPos + 5, wstrTmp.length() - nPos - 5);
		}
	}

	FLOG(L"CCMDParser::Parse end");
	return;
}

void CCMDParser::ParseOldCmd(wstring wstrCmd)
{
	// �������еĲ��淶�������У����˼���ָ���⣬��Ϊ��һָ��
	// ������,����ָ���Ϊ���֣�1��/active ֻ�����������2��/active: ���������3��/active:{ �������
	// ֻ������ָ�����ڴ�����ʾǰ��������ֻ���������Ͳ���Э��	
	if (0 == wstrCmd.find(CMD_RUN_ACTIVE))
	{
		m_wstrCmd_Run = CMD_RUN_ACTIVE;
		if (wstrCmd.length() > 7)
		{
			m_bPlay = true;
			wstring wstrTmp = wstrCmd;
			int index = wstrTmp.find(L":");
			wstrTmp = wstrTmp.substr(index + 1, wstrTmp.length());			
			m_wstrCmd_Play = LeTools::replace(wstrTmp, L"\\", L"");
		}
	}

	// ������
	if (0 == wstrCmd.length())
	{
		m_wstrCmd_Run = L"";
		return;
	}
	if (0 == wstrCmd.find(CMD_RUN_AUTORUN))
	{
		m_wstrCmd_Run = CMD_RUN_AUTORUN;
		return;
	}
	if (0 == wstrCmd.find(CMD_RUN_DESKTOP))
	{
		m_wstrCmd_Run = CMD_RUN_DESKTOP;
		return;
	}
	if (0 == wstrCmd.find(CMD_RUN_STARTMENU))
	{
		m_wstrCmd_Run = CMD_RUN_STARTMENU;
		return;
	}
	if (0 == wstrCmd.find(CMD_RUN_TASKBAR))
	{
		m_wstrCmd_Run = CMD_RUN_TASKBAR;
		return;
	}
	if (0 == wstrCmd.find(CMD_RUN_PCM))
	{
		m_wstrCmd_Run = CMD_RUN_PCM;
		return;
	}
	if (0 == wstrCmd.find(CMD_RUN_RUNNEW))
	{
		m_wstrCmd_Run = CMD_RUN_RUNNEW;
		CLeReport::GetInstance()->SendRTD_Eevent(RTD_UPDATE, "1", "4replace");
		return;
	}	
	m_wstrCmd_Run = wstrCmd;

	// ������
	if (0 == wstrCmd.find(CMD_PLAY_LM))
	{
		if (wstrCmd.length() > 7)
		{
			m_bPlay = true;
			int index = wstrCmd.find(L":");
			wstring wstrtmp = wstrCmd.substr(index + 1, wstrCmd.length());
			m_wstrCmd_Play = LeTools::replace(wstrtmp, L"\\", L"");
			// �ִ����𲥷�
			CLeReport::GetInstance()->SendRTD_Eevent("1", "1", "lewinplay");
		}
	}
	if (0 == wstrCmd.find(L"fsp") || 0 == wstrCmd.find(L"http://") || PathFileExists(wstrCmd.c_str()))
	{
		m_bPlay = true;
		m_wstrCmd_Play = wstrCmd;
	}
	if (0 == wstrCmd.find(L"LePlayer://") || 0 == wstrCmd.find(L"leplayer://"))
	{
		// web����˲���
		m_wstrCmd_Run = L"web";
		int index = wstrCmd.find(L"://");
		m_bPlay = true;
		m_wstrCmd_Play = wstrCmd.substr(index + 3, wstrCmd.length());
		wstring wtmp = m_wstrCmd_Play;
		string detmp = LeTools::urlDecode(LeTools::ws2s(wtmp).c_str());
		m_wstrCmd_Play = LeTools::s2ws(detmp);
	}
	// �����ޱ�ʶ�Ĳ���json
	if (ParsePlayJson(wstrCmd))
	{
		m_wstrCmd_Run = L"popup";
		m_bPlay = true;
		m_wstrCmd_Play = wstrCmd;
	}
	else
	{
		wstring wstrTmp = LeTools::replace(wstrCmd, L"\\", L"");
		if (ParsePlayJson(wstrTmp))
		{
			m_wstrCmd_Run = L"popup";
			m_bPlay = true;
			m_wstrCmd_Play = wstrTmp;
		}
	}

	// ������
	if (-1 != wstrCmd.find(CMD_FUNC_PLAY))
	{
		m_bFunc = true;
		m_wstrCmd_Func = CMD_FUNC_PLAY;
	}
	if (-1 != wstrCmd.find(CMD_FUNC_PAUSE))
	{
		m_bFunc = true;
		m_wstrCmd_Func = CMD_FUNC_PAUSE;
	}
	
	return;
}

bool CCMDParser::ParsePlayJson(wstring wstrCmd)
{
	string strInfo = LeTools::ws2s(wstrCmd);
	Json::Reader jr;
	Json::Value jv;	
	if (jr.parse(strInfo, jv))
		return true;	
	return false;
}

bool CCMDParser::IsRun()
{
	return m_bRun;
}

bool CCMDParser::IsPlay()
{
	return m_bPlay;
}

bool CCMDParser::IsFunc()
{
	return m_bFunc;
}

bool CCMDParser::IsOpenUrl()
{
	return m_bUrl;
}

std::wstring CCMDParser::GetRunCmd()
{
	return m_wstrCmd_Run;
}

std::wstring CCMDParser::GetPlayCmd()
{
	return m_wstrCmd_Play;
}

std::wstring CCMDParser::GetFunCCmd()
{
	return m_wstrCmd_Func;
}

std::wstring CCMDParser::GetUrlCmd()
{
	return m_wstrCmd_Url;
}
