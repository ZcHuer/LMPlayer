#include "stdafx.h"
#include "ffplayer.h"
#include "../SDK/Flog/FileLog.h"

// 播放器窗口的父窗口的窗口函数
WNDPROC g_PlayerParentWndFunc = NULL;
CFFPlayer* CFFPlayer::m_pThis = nullptr;

static bool MsgWait(HANDLE hObject, DWORD dwTime)
{
	DWORD  dwCur = GetTickCount();
	dwTime == INFINITE ? 0 : dwTime;

	while (true)
	{
		DWORD result = MsgWaitForMultipleObjects(1, &hObject, FALSE, dwTime, QS_ALLINPUT);
		if (result == (WAIT_OBJECT_0))
			return true;

		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			bool bQuit = (msg.message == WM_QUIT);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (bQuit)
				return false;
		}
		else
		{
			//防止CPU处理不过来
			::Sleep(10);
		}
		if (dwTime && (GetTickCount() - dwCur >= dwTime))
		{
			break;
		}
	}
	return false;
}

LRESULT PlayerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (NULL == CFFPlayer::m_pThis)
		return S_FALSE;

	// 截获了父窗口的窗口函数，先把size消息处理过后，剩下的消息，再交给父窗口处理
	if (uMsg == WM_SIZE)
	{
		RECT rcWnd;
		::GetWindowRect(CFFPlayer::m_pThis->m_hWndParent, &rcWnd);
		if (ePS_Stoped == CMsgWnd::m_ePlayerStatus || ePS_Init == CMsgWnd::m_ePlayerStatus)
			CFFPlayer::m_pThis->m_PlayerWnd.SetWindowPos(NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top - CFFPlayer::m_pThis->m_nCtrlHeight, SWP_NOZORDER | SWP_HIDEWINDOW);
		else
			CFFPlayer::m_pThis->m_PlayerWnd.SetWindowPos(NULL, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top - CFFPlayer::m_pThis->m_nCtrlHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
	}
	else if (WM_DESTROY == uMsg)
	{
		PostQuitMessage(0);
 	}

	return (CallWindowProc((WNDPROC)g_PlayerParentWndFunc, hwnd, uMsg, wParam, lParam));
}

unsigned int WINAPI _StartPlayCmdThread(void *param)
{
	CFFPlayer * pFFPlayer = (CFFPlayer *)param;
	if (pFFPlayer)
	{
		pFFPlayer->PlayCmdThread();
	}
	return 0;
}

CFFPlayer::CFFPlayer() :
	m_hWndParent(NULL),
	m_bMute(false),
	m_nCtrlHeight(0),
	m_duration(0),
	m_seekpos(0),
	m_nVolume(30),
	m_nAudioChannel(0),
	m_playCmdThreadID(0),
	m_hEvent_Create(NULL),
	m_hEvent_Init(NULL),
	m_hEvent_Stop(NULL)
{
	m_pThis = this;
	m_nflags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_WINDOW_BORDERLESS;
}

CFFPlayer::~CFFPlayer()
{	
	Stop();
	DestoryPlayCmdThread();
	m_pThis = NULL;
}

bool CFFPlayer::CreatePlayer(HWND hWndParent, LPRECT lpRect)
{
	FLOG(L"CreatePlayer begin");
	m_MsgWnd.CreateMsgWnd(hWndParent);	
	m_hWndParent = hWndParent;
	if (NULL == m_hEvent_Create)
		m_hEvent_Create = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hPlayCmdThread = (HANDLE)_beginthreadex(NULL, 0, _StartPlayCmdThread, (void *)this, 0, &m_playCmdThreadID);

	if (!MsgWait(m_hEvent_Create, 5000))
		return false;

	if (NULL == m_hEvent_Init)
		m_hEvent_Init = ::CreateEvent(NULL, FALSE, FALSE, NULL);

	RECT* rc = new RECT;
	rc->left = lpRect->left;
	rc->top = lpRect->top;
	rc->right = lpRect->right;
	rc->bottom = lpRect->bottom;
	PostThreadMessage(m_playCmdThreadID, WM_PlayerThread_Init, (WPARAM)rc, 0);

	if (!MsgWait(m_hEvent_Init, 5000))
		return false;

	FLOG(L"CreatePlayer end");
	return true;
}

void CFFPlayer::AttachCallBack(IPlayerCallBack* pCallBack)
{
	return m_MsgWnd.AttachCallBack(pCallBack);
}

void CFFPlayer::PlayCmdThread()
{
	FLOG(L"PlayCmdThread begin");
	MSG msg;
	PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE);
	::SetEvent(m_hEvent_Create);
	bool bQuit = false;
	CoInitialize(NULL);
	while (!bQuit && (GetMessage(&msg, 0, 0, 0) > 0))
	{
		if (msg.hwnd != NULL)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		switch (msg.message)
		{
		case WM_PlayerThread_Init:
		{
			FLOG(L"WM_PlayerThread_Init begin");
			RECT* pRc = (RECT*)msg.wParam;
			if (pRc)
			{
				m_PlayerWnd.CreateEx(m_hWndParent, pRc/*RECT(pRc->left, pRc->top, pRc->right, pRc->bottom)*/, WS_CHILD);
				FLOG(L"WM_PlayerThread_Init : %d - %d : %d : %d : %d", m_hWndParent, pRc->left, pRc->top, pRc->right, pRc->bottom);
				::EnableWindow(m_PlayerWnd.m_hWnd, FALSE);
				//截取父窗口消息，以便跟随父窗口改变窗口大小
				g_PlayerParentWndFunc = (WNDPROC)::SetWindowLong(m_hWndParent, GWL_WNDPROC, (LRESULT)PlayerWndProc);
				::SetEvent(m_hEvent_Init);
				delete pRc;
			}
			FLOG(L"WM_PlayerThread_Init end");
		}
		break;
		case WM_PlayerThread_Open:
		{
			FLOG(L"WM_PlayerThread_Open begin");
			// 线程循环在这里会被sdleventloop接管，而sdlloop是走消息的，所以线程消息现在不能用了，只有在stop之后，才可以重新使用此线程循环。
			char* p = (char*)msg.wParam;
			Open(p);
			delete p;
			::SetEvent(m_hEvent_Stop);
			FLOG(L"WM_PlayerThread_Open end");
		}
		break;
		case WM_PlayerThread_Quit:
		{
			FLOG(L"WM_PlayerThread_Quit begin");
			m_PlayerWnd.DestroyWindow();
			bQuit = true;
			FLOG(L"WM_PlayerThread_Quit end");
		}
		break;
		}
	}
	CoUninitialize();
	FLOG(L"PlayCmdThread end");
}

void CFFPlayer::DestoryPlayCmdThread()
{
	FLOG(L"CFFPlayer::DestoryPlayCmdThread() begin");
	if (m_playCmdThreadID)
	{
		PostThreadMessage(m_playCmdThreadID, WM_PlayerThread_Quit, 0, 0);
		m_playCmdThreadID = 0;
	}		

	if (m_hEvent_Create != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent_Create);
		m_hEvent_Create = INVALID_HANDLE_VALUE;
	}
	if (m_hEvent_Init != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent_Init);
		m_hEvent_Init = INVALID_HANDLE_VALUE;
	}
	if (m_hEvent_Stop != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hEvent_Stop);
		m_hEvent_Stop = INVALID_HANDLE_VALUE;
	}
	FLOG(L"CFFPlayer::DestoryPlayCmdThread() end");
}

void CFFPlayer::ReWndSize(int nLeft, int nTop, int nRight, int nBottom)
{
	FLOG(L"CFFPlayer::ReWndSize begin");
	if (NULL == m_hWndParent)
		return;

	m_nCtrlHeight = nBottom;
	RECT rcWnd;
	::GetWindowRect(m_hWndParent, &rcWnd);

	if (NULL == m_PlayerWnd.m_hWnd)
	{
		FLOG(L"CFFPlayer::ReWndSize NULL == m_PlayerWnd.m_hWnd return");
		return;
	}
	if (ePS_Stoped == CMsgWnd::m_ePlayerStatus || ePS_Opening == CMsgWnd::m_ePlayerStatus)
		m_PlayerWnd.SetWindowPos(HWND_TOP, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top - m_nCtrlHeight, SWP_NOZORDER | SWP_HIDEWINDOW);
	else
		m_PlayerWnd.SetWindowPos(HWND_TOP, 0, 0, rcWnd.right - rcWnd.left, rcWnd.bottom - rcWnd.top - m_nCtrlHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
	FLOG(L"CFFPlayer::ReWndSize end");
	return;
}

ePlayStatus CFFPlayer::GetPlayStatus()
{
	return CMsgWnd::m_ePlayerStatus;
}

void CFFPlayer::OpenAndPlay(const char* pch)
{
	FLOG(L"CFFPlayer::OpenAndPlay begin");

	if (NULL == m_hEvent_Stop)
		m_hEvent_Stop = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	ResetEvent(m_hEvent_Stop);

	int nLen = strlen(pch);
	char* p = new char[nLen + 1];
	memset(p, 0, (nLen + 1));
	strcpy_s(p, nLen + 1, pch);

	PostThreadMessage(m_playCmdThreadID, WM_PlayerThread_Open, (WPARAM)p, 0);
	FLOG(L"CFFPlayer::OpenAndPlay end");
	return;
}

void CFFPlayer::Play()
{
	FLOG(L"CFFPlayer::Play begin");
	if (ePS_Playing == GetPlayStatus())
		return;

	m_MsgWnd.SetPlayStatus(ePS_Playing);

	// 为了防止暂停时seek时播放一帧音频比较难听，暂停时，自动静音，播放时还原静音设置。
	SDL_Event SE0;
	SE0.type = SDL_USEREVENT;
	SE0.user.code = SDL_USEREVENT_TYPE_SDL_MUTE;
	SE0.user.data1 = (void*)m_bMute;
	SDL_PushEvent(&SE0);

	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_PLAYPAUSE;
	SDL_PushEvent(&SE);

	FLOG(L"CFFPlayer::Play end");
	
	return;
}

void CFFPlayer::Pause()
{
	FLOG(L"CFFPlayer::Pause begin");
	if (ePS_Pause == GetPlayStatus())
		return;

	m_MsgWnd.SetPlayStatus(ePS_Pause);
	
	// 为了防止暂停时seek时播放一帧音频比较难听，暂停时，自动静音，播放时还原静音设置。
	SDL_Event SE0;
	SE0.type = SDL_USEREVENT;
	SE0.user.code = SDL_USEREVENT_TYPE_SDL_MUTE;
	SE0.user.data1 = (void*)1;
	SDL_PushEvent(&SE0);

	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_PLAYPAUSE;
	SDL_PushEvent(&SE);

	FLOG(L"CFFPlayer::Pause end");
	return;
}

void CFFPlayer::Stop()
{
	FLOG(L"CFFPlayer::Stop begin");
	CMsgWnd::SetPlayStatus(ePS_Stoped);
	if (NULL == m_hEvent_Stop || g_bStop)
		return;
	
	SDL_Event SE;
	SE.type = FF_QUIT_EVENT;
	SDL_PushEvent(&SE);

	// 设置一个事件，等上次播放退出，不然线程是阻塞的，之后的线程消息会失败。
	MsgWait(m_hEvent_Stop, 2000);
	g_bStop = true;

	CMsgWnd::SetPlayStatus(ePS_Stoped);

	FLOG(L"CFFPlayer::Stop end");
	return;
}

void CFFPlayer::Seek(LONGLONG dbPos)
{
	FLOG(L"CFFPlayer::Seek begin");
	m_seekpos = dbPos;
	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_SDL_SEEK;
	SE.user.data1 = (void*)dbPos;
	SDL_PushEvent(&SE);
	FLOG(L"CFFPlayer::Seek end");
	return;
}

void CFFPlayer::SetVolume(int nVolume)
{
	FLOG(L"CFFPlayer::SetVolume begin");
	m_nVolume = max(nVolume, 0);
	m_nVolume = min(nVolume, 100);

	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_SDL_VOLUME;
	SE.user.data1 = (void*)(int(m_nVolume*SDL_MIX_MAXVOLUME / 100));
	SDL_PushEvent(&SE);

	FLOG(L"CFFPlayer::SetVolume end");
	return;
}

int CFFPlayer::GetVolume()
{
	return m_nVolume;
}

void CFFPlayer::SetMute(bool bMute)
{
	if (m_bMute == bMute)
		return;

	// 静音处理
	m_bMute = bMute;
	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_SDL_MUTE;
	SE.user.data1 = (void*)m_bMute;
	SDL_PushEvent(&SE);
	return;
}

bool CFFPlayer::GetMute()
{
	return m_bMute;
}

void CFFPlayer::SetPlayRate(double dbRate)
{
	return;
}

double CFFPlayer::GetPlayRate()
{
	return 1;
}

bool CFFPlayer::SetVideoSize(long pWidth, long pHeight)
{
	return true;
}

bool CFFPlayer::GetVideoSize(long *pWidth, long* pHeight)
{
	return true;
}

void CFFPlayer::SetSubtitlefile(const char* lpPath)
{
	return;
}

int CFFPlayer::GetSubtitleCount()
{
	return 0;
}

void CFFPlayer::SetSubtitleSpu(unsigned nSpu)
{
	return;
}

int CFFPlayer::GetSubtitleSpu()
{
	return 0;
}

void CFFPlayer::SetAspectRatio(int nScale)
{
	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_SDL_SCALE;
	SE.user.data1 = (void*)nScale;
	SDL_PushEvent(&SE);
	return;
}

int CFFPlayer::GetAspectRatio()
{
	return 0;
}

bool CFFPlayer::SetEventMessage(HWND hEventWnd, UINT uEventMsg)
{
	return true;
}

bool CFFPlayer::OnMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void CFFPlayer::SetAudioChannel(int nChannel)
{
	if (m_nAudioChannel != nChannel)
		m_nAudioChannel = nChannel;

	SDL_Event SE;
	SE.type = SDL_USEREVENT;
	SE.user.code = SDL_USEREVENT_TYPE_SDL_AUDIOCHANNEL;
	SE.user.data1 = (void*)m_nAudioChannel;
	SDL_PushEvent(&SE);

	return;
}

int CFFPlayer::GetAudioChannel()
{
	return m_nAudioChannel;
}

void CFFPlayer::Close()
{
	return;
}

void CFFPlayer::Quit()
{
	FLOG(L"CFFPlayer::Quit begin");
	Stop();
	if (m_playCmdThreadID)
	{
		PostThreadMessage(m_playCmdThreadID, WM_PlayerThread_Quit, 0, 0);
		m_playCmdThreadID = 0;
	}
	FLOG(L"CFFPlayer::Quit end");
	return;
}

void CFFPlayer::Open(char* p)
{
	FLOG(L"CFFPlayer::Open begin");
	m_duration = 0;
	init_dynload();
	av_log_set_flags(AV_LOG_SKIP_REPEATED);
	//parse_loglevel(argc, argv, options);

	/* register all codecs, demux and protocols */
#if CONFIG_AVDEVICE
	avdevice_register_all();
#endif
	avformat_network_init();
	init_opts();
	signal(SIGINT, sigterm_handler); /* Interrupt (ANSI).    */
	signal(SIGTERM, sigterm_handler); /* Termination (ANSI).  */

	if (SDL_Init(m_nflags))
	{
		av_log(NULL, AV_LOG_FATAL, "Could not initialize SDL - %s\n", SDL_GetError());
		av_log(NULL, AV_LOG_FATAL, "(Did you set the DISPLAY variable?)\n");
		return;
	}

	// 忽略系统消息
	SDL_EventState(SDL_SYSWMEVENT, SDL_IGNORE);
	SDL_EventState(SDL_USEREVENT, SDL_IGNORE);

	//指定SDL显示在播放器窗口
	window = SDL_CreateWindowFrom(m_PlayerWnd.m_hWnd);
	RECT rcPlayer;
	::GetClientRect(m_PlayerWnd.m_hWnd, &rcPlayer);
	screen_width = rcPlayer.right - rcPlayer.left;
	screen_height = rcPlayer.bottom - rcPlayer.top;
	//window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, default_width, default_height, SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
	if (window) 
	{
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!renderer) {
			av_log(NULL, AV_LOG_WARNING, "Failed to initialize a hardware accelerated renderer: %s\n", SDL_GetError());
			renderer = SDL_CreateRenderer(window, -1, 0);
		}
		if (renderer) {
			if (!SDL_GetRendererInfo(renderer, &renderer_info))
				av_log(NULL, AV_LOG_VERBOSE, "Initialized %s renderer.\n", renderer_info.name);
		}
	}
	
	if (!window || !renderer || !renderer_info.num_texture_formats) 
	{
		av_log(NULL, AV_LOG_FATAL, "Failed to create window or renderer: %s", SDL_GetError());
		do_exit(NULL);
		return;
	}

	//DWORD dw = GetCurrentThreadId();
	input_filename = p;
	if (!input_filename)
	{
		show_usage();
		av_log(NULL, AV_LOG_FATAL, "An input file must be specified\n");
		av_log(NULL, AV_LOG_FATAL,
			"Use -h to get full help or, even better, run 'man %s'\n", ""/*program_name*/);
		//exit(1);
		return;
	}

	av_init_packet(&flush_pkt);
	flush_pkt.data = (uint8_t *)&flush_pkt;

	VideoState* is = stream_open(input_filename, file_iformat, m_MsgWnd.GetHwnd());
	if (!is)
	{
		av_log(NULL, AV_LOG_FATAL, "Failed to initialize VideoState!\n");
		do_exit(NULL);
		return;
	}

	// 每次open都要带着音量设置
	is->muted = m_bMute;
	is->audio_volume = m_nVolume;

	event_loop(is);

	FLOG(L"CFFPlayer::Open end");
	return;
}
