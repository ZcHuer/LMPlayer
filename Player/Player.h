// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 PLAYER_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// PLAYER_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
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

enum ePlayStatus
{
	ePS_Init,
	ePS_Opening,
	ePS_Playing,
	ePS_Pause,
	ePS_Seeking,
	ePS_Stoped
};

class IPlayerCallBack
{
public:
	virtual	void OnPlayStart(int nDuration) = 0;
	virtual	void OnPlaying(int nPlayPos, int nDuration) = 0;
	virtual	void OnPlayEnd() = 0;
	virtual void OnFileInvalid() = 0;
};

// 此类是从 Player.dll 导出的
class PLAYER_API CPlayer
{
public:
	CPlayer(void);
	~CPlayer();
	bool CreatePlayer(HWND hWndParent, LPRECT lpRect);				// 创建播放器，内部启动线程
	void AttachCallBack(IPlayerCallBack* pCallBack);				// 设置回调指针
	ePlayStatus GetPlayStatus();
	void ReWndSize(int nLeft, int nTop, int nRight, int nBottom);	// 调整窗口大小（向内偏移量）	
	void OpenAndPlay(const char* pch);									// 播放相关-可外部调用	
	void Play();
	void Pause();
	void Seek(LONGLONG dbPos);
	void SetPlayRate(double dbRate);	
	double GetPlayRate();
	void SetVolume(int nVolume);
	int GetVolume();
	void SetMute(bool bMute);
	bool GetMute();
	void SetAudioChannel(int nChannel);
	int GetAudioChannel();
	bool SetVideoSize(long pWidth, long pHeight);
	bool GetVideoSize(long *pWidth, long* pHeight);
	void SetSubtitlefile(const char* lpPath);						//添加字幕
	int GetSubtitleCount();											//获取可用字幕数量
	void SetSubtitleSpu(unsigned nSpu);
	int GetSubtitleSpu();
	void SetAspectRatio(int nScale);								// 设置屏幕纵横比
	int GetAspectRatio();											//获取屏幕纵横比
	void Stop();
	void Quit();
};
