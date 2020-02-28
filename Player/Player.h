// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� PLAYER_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// PLAYER_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#pragma once
#ifdef PLAYER_EXPORTS
#define PLAYER_API __declspec(dllexport)
#else
#define PLAYER_API __declspec(dllimport)
#endif

#define SCALE_Original		0
#define SCALE_16X9			1
#define SCALE_4X3			2
#define SCALE_fill			3

// ����״̬
enum ePlayStatus
{
	ePS_Init,
	ePS_Opening,
	ePS_Playing,
	ePS_Pause,
	ePS_Seeking,
	ePS_Stoped
};

// ����״̬�ص��ӿ�
class IPlayerCallBack
{
public:
	virtual	void OnPlayStart(int nDuration) = 0;
	virtual	void OnPlaying(int nPlayPos, int nDuration) = 0;
	virtual	void OnPlayEnd() = 0;
	virtual void OnFileInvalid() = 0;
};

// �����Ǵ� Player.dll ������
class PLAYER_API CPlayer
{
public:
	CPlayer(void);
	~CPlayer();
	bool CreatePlayer(HWND hWndParent, LPRECT lpRect);				// �������������ڲ������߳�
	void AttachCallBack(IPlayerCallBack* pCallBack);				// ���ûص�ָ��
	ePlayStatus GetPlayStatus();									// �õ�����״̬
	void ReWndSize(int nLeft, int nTop, int nRight, int nBottom);	// �������ڴ�С������ƫ������	
	void OpenAndPlay(const char* pch);								// �������-���ⲿ����	
	void Play();													// ����
	void Pause();													// ��ͣ
	void Seek(LONGLONG dbPos);										// Seek
	void SetPlayRate(double dbRate);								// ���ò�������
	double GetPlayRate();											// ��ȡ��������
	void SetVolume(int nVolume);									// ��������
	int GetVolume();												// �õ���ǰ����
	void SetMute(bool bMute);										// ���þ���
	bool GetMute();													// ��ȡ�Ƿ���
	void SetAudioChannel(int nChannel);								// ������Ƶ����
	int GetAudioChannel();											// �õ���Ƶ����
	bool SetVideoSize(long pWidth, long pHeight);					// ������Ƶ�ߴ�
	bool GetVideoSize(long *pWidth, long* pHeight);					// �õ���Ƶ�ߴ�
	void SetSubtitlefile(const char* lpPath);						// �����Ļ
	int GetSubtitleCount();											// ��ȡ������Ļ����
	void SetSubtitleSpu(unsigned nSpu);								// ������Ļ�ٶ�
	int GetSubtitleSpu();											// �õ���Ļ�ٶ�
	void SetAspectRatio(int nScale);								// ������Ļ�ݺ��
	int GetAspectRatio();											// ��ȡ��Ļ�ݺ��
	void Stop();													// ֹͣ
	void Quit();													// �˳�
};
