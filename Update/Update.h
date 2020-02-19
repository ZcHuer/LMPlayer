// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� UPDATE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// UPDATE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef UPDATE_EXPORTS
#define UPDATE_API __declspec(dllexport)
#else
#define UPDATE_API __declspec(dllimport)
#endif

#include <string>
using namespace std;

#define Error_Download  0
#define Error_Unzip		1
#define Error_File		2

// �򴰿ڷ��͵���Ϣ
enum eUpdateMsg
{
	eUPDATE_Progress = WM_USER + 1,		// ���ؽ���
	eUPDATE_Success,					// �ɹ�
	eUPDATE_Error_Download,				// ����ʧ��
	eUPDATE_Error_Unzip,				// ����ʧ��
	eUPDATE_Error_File,					// �ļ�����
};

struct sUpdateInfo
{
	int id;
	int tvInfoId;
	string appName;
	string appPackage;
	string appSize;
	string appLoadUrl;
	string versionCode;
	string versionName;
	string versionDesc;
	int statusCode;
	int statusCount;
	string miniVersion;
	string updateType;
	long createTime;
	long updateTime;
	string md5;
};

// �����Ǵ� Update.dll ������
class UPDATE_API CUpdate 
{
public:
	CUpdate();
	~CUpdate();
	// ��ѯ�Ƿ���������
	bool IsDownLoading();
	// ��ѯ��������
	bool GetUpdateConfig(sUpdateInfo& sInfo);
	// �����������������Ҫ���̣߳��������������ķ���ֵ��Ĭ�ϲ����̣߳���Ϊû�лص��Ĵ��ھ����ֻ��������������ֵ
	bool DownloadAndUnzip(LPCSTR lpcstr_url, LPCSTR lpcstr_vername, bool bThread = false, HWND hWnd = NULL);
	// ȡ������
	void CancelDownload();
	// �󶨻ص����
	bool SetCallBackHwnd(HWND hWnd);
};