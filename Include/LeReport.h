#include <string>
using namespace std;
// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� LEREPORT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// LEREPORT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef LEREPORT_EXPORTS
#define LEREPORT_API __declspec(dllexport)
#else
#define LEREPORT_API __declspec(dllimport)
#endif

// �����Ǵ� LeReport.dll ������
class  CLeReport
{
public:
	// TODO:  �ڴ�������ķ�����
	static CLeReport* GetInstance();
	static void Release();
	~CLeReport();

	/* ����ӿں��� */
	void SetDataServer(const char* pchServer, const char* pchInterFace);		// �����ϱ���ַ
	void SetVersion(const char* pchVer);										// �汾
	void SetLenovoId(const char* pchLenovoId);									// �û�id
	void SetchannelID(const string channelID);									// ��������ID
	void SetIfVip(string sVip);
	void SetLaunchFrom(string sLaunch);
	void SetExternRd(string sCmdRd);

	string GetOsType();
	string GetDeviceStyle();
	// ���ͱ����ռ��ĳ�������
	void SendCollectData();
	// ʵʱ��������
	void SendRTD_Eevent(const string sEventID, const string sEvnetype, const string sEventDes,
		string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);
	bool SendRTD_Eeventsync(const string sEventID, const string sEvnetype, const string sEventDes,
		string videoId = "", string format = "", int nSourceType = 0, __int64 nUserTime = 0);

	void SendRTD_Page(const string pageId, const string pageLevel, long stopTime, int binto = 1, int nSourceType = 0, string sDes = "ҳ�����");
	void SendRTD_Play(const string sEventID, const string sEvnetype, const string sEventDes, enum eSourceType etype, struct sReportData &old);
	void SendRTD_Login(const string sEventID, const string sEvnetype, const string sEventDes, const string sPhone, const string mail, const string sname);
	void SendRTD_Adv(const string sEventID, const string sEvnetype, const string sEventDes, struct AdStruct &sAdv, enum eSourceType etype, struct sReportData &old);
	// ��¼�������ݵ�����-����
	void SaveData_Count(int lType, int lValue = 1);
	void SaveData_Bool(int lType, bool bValue);
	void SaveData_String(int lType, const char* pchValue);
	
	void QuitThread();

private:
	CLeReport(void);
	static CLeReport* m_pInstance;	
};