#pragma once
/*
	此定义为整个leplayer所有程序/模块共用的定义；
	为了保持统一，避免出现重复和错误，如果要添加，也必须添加在这里后，再使用宏；
	此定义，只可增加，不可修改。
*/

// js调用c++命令
#define JSCALLCPLUS_CMD_USERINFOCHANGE					L"0"		// 用户信息改变
#define JSCALLCPLUS_CMD_LOGIN							L"1"		// 弹出登陆框
#define JSCALLCPLUS_CMD_FUNTV_DETAIL					L"2"		// 风行-详情页地址
#define JSCALLCPLUS_CMD_FUNTV_PLAY						L"3"		// 风行-播放
#define JSCALLCPLUS_CMD_CREATESHUTCUT					L"4"		// 生成快捷方式
#define JSCALLCPLUS_CMD_LOGOUT							L"5"		// 退出登陆
#define JSCALLCPLUS_CMD_REFRESH							L"6"		// 刷新
#define JSCALLCPLUS_CMD_FUNTV_WEBPLAY					L"7"		// 风行web播放
#define JSCALLCPLUS_CMD_CLOSEWINDOWS					L"8"		// 关闭窗口
#define JSCALLCPLUS_CMD_JSREADY							L"9"		// Js就绪
#define JSCALLCPLUS_CMD_OPENLINK						L"50"		// 打开外部链接
#define JSCALLCPLUS_CMD_LEPLAYERHOMEURL					L"51"		// 联想视频端内打开url
#define JSCALLCPLUS_CMD_ADDCOUNT						L"52"		// 记录统计数据
#define JSCALLCPLUS_CMD_ADDBOOL							L"53"		// 记录布尔数据
#define JSCALLCPLUS_CMD_ADDSTRING						L"54"		// 记录字符串
// 优酷
#define JSCALLCPLUS_CMD_YOUKU_PLAY						L"100"		// 优酷-播放
#define JSCALLCPLUS_CMD_YOUKU_PLAY_VIP_CONTINUE			L"101"		// 优酷-购买后续播(从优酷端触发的购买，js支付成功后要续播)
#define JSCALLCPLUS_CMD_YOUKU_LOGIN_PLAY				L"102"		// 优酷-登陆后续播(vip片源，js会调登陆，登陆后要续播)
#define JSCALLCPLUS_CMD_YOUKU_PAY						L"103"		// 优酷-购买信息同步（无片源信息时，仅支付成功后，要通知给客户端）
#define JSCALLCPLUS_CMD_YOUKU_GETPLAYLOG				L"104"		// 优酷-播放记录获取
#define JSCALLCPLUS_CMD_YOUKU_DELETEPLAYLOG				L"105"		// 优酷-播放记录删除
#define JSCALLCPLUS_CMD_YOUKU_PLAYLOGANDPLAY			L"106"		// 优酷-播放记录播放

// 播放类（为了多cp支持，新增的播放类指令id） 1000-1100
#define  JSCALLCPLUS_CMD_PLAY_CP_BEGIN					1000		// 播放指令范围起始值
#define  JSCALLCPLUS_CMD_KANKAN_DETAIL					1001		// 看看新闻-显示详情页
#define  JSCALLCPLUS_CMD_KANKAN_PLAY					1002		// 看看新闻-详情页调播放
#define  JSCALLCPLUS_CMD_WEB_PLAY						1003		// 网页播放
#define  JSCALLCPLUS_CMD_CP_UPDATE						1099		// 强制升级
#define  JSCALLCPLUS_CMD_PLAY_CP_End					1100		// 播放指令范围结束值

// c++调用js命令
#define CPLUSTOJS_SETLOGIN								0			// 登陆
#define CPLUSTOJS_SETLOGOUT								1			// 退出
#define CPLUSTOJS_SETMAC								2			// 设置mac
#define CPLUSTOJS_PLAYNEXTONE							3			// 播放下一个
#define CPLUSTOJS_PLAYPREONE							4			// 播放上一个
#define CPLUSTOJS_YOUKU_VIPCHARGE_USER					5			// 优酷-vip会员充值
#define CPLUSTOJS_YOUKU_VIPCHARGE_VIDEO					6			// 优酷-vip单片充值
#define CPLUSTOJS_YOUKU_ISVIP							7			// 优酷-是否vip
#define CPLUSTOJS_YOUKU_VIP_EXPIRETIME					8			// 优酷-vip有效期
#define CPLUSTOJS_YOUKU_VOUCHER							9			// 优酷-观影券
#define CPLUSTOJS_YOUKU_PLAYHISTROY_ALL					10			// 优酷-播放记录-全部
#define CPLUSTOJS_YOUKU_PLAYHISTROY_VIDEO				11			// 优酷-播放记录-单片
#define CPLUSTOJS_YOUKU_COLLECTION						12			// 优酷-收藏记录
#define CPLUSTOJS_SETADVINFO							13			// 设置弹窗广告信息
#define CPLUSTOJS_SET_VERSION							14			// 向JS设置准确版本号
#define CPLUSTOJS_SET_LENOVOID_ENCRYPT					15			// 向JS设置加密后的lenovoid


// 登陆发起原因定义
#define LOGIN_LENOVO_MENU								0			// 菜单发起登陆
#define LOGIN_LENOVO_JS									1			// JS发起登陆
#define LOGIN_LENOVO_JS_PLAY							2			// JS调用播放发起的登陆，会传视频信息过来
#define LOGIN_LENOVO_PLAYHISTROY						3			// 查看播放记录发起登陆
#define LOGIN_LENOVO_PLAYHISTROY_MORE					4			// 播放记录查看更多发起登陆
#define LOGIN_LENOVO_FAVORITE							5			// 查看收藏记录发起登陆
#define LOGIN_LENOVO_FAVORITE_MORE						6			// 收藏记录查看更多发起登陆
#define LOGIN_LENOVO_BUYVIP								7			// 联想端购买vip发起登陆
#define LOGIN_YK_BUYVIP									8			// 优酷-购买vip发起登陆
#define LOGIN_YK_BUYVIDEO								9			// 优酷-购买单片发起登陆
#define LOGIN_CHANGEUSER								10			// 客户端切换用户触发登录

// 登陆方式
#define LOGIN_TYPE_PHONE_PSWD							1			// 手机号+密码
#define LOGIN_TYPE_PHONE_CODE							2			// 手机号+验证码
#define LOGIN_TYPE_EMAIL_PSWD							3			// 邮箱+密码
#define LOGIN_TYPE_EMAIL_CODE							4			// 邮箱+验证码

// 播放的发起方式
enum eSignalType
{
	eSignalType_UnKnown,		// 未知
	eSignalType_OpenFile,		// 打开文件
	eSignalType_PlayList,		// 播放列表
	eSignalType_HomeUrl,		// 首页
	eSignalType_DetailUrl,		// 详情页
	eSignalType_CmdLine,		// 命令行
	eSignalType_DrogFile		// 拖拽
};

// 片源类型
enum eSourceType
{
	eSourceType_Unknown,		// 未知
	eSourceType_Local,			// 本地
};

// 命令行发送方式
enum eCmdSource
{
	eeCmdSource_Run,
	eeCmdSourceCopyData
};

// 定时器类型
enum MyTimer
{
	ET_BEGIN = 50,
	ET_SHOW_VOLUME,
	T_LIST_SWITCH,
	T_HIDE_COMPLETE,
	T_HIDE_SAVE,
	T_HIDE_POPUP,
	ET_HIDETIG,				//隐藏tig
	ET_HIDEHEAD,			//隐藏浮动栏，控制栏和浮动栏一起隐藏
	ET_TIME_GET,
	ET_CLICK_PAUSE,			//点击屏幕暂停继续
	ET_FLASH_PLAY,			//flash播放
	ET_HIDE_EDITTIP,		//隐藏edit tip
	ET_MAGNET_END,
	ET_TASK_MSG,
	ET_CHECK_SIZE,
	ET_VerificationCode,	//验证码
	ET_SEARCH_HELP,			//搜索帮助
	ET_RELOAD_LIST,			//重加载列表
	ET_CHECK_EDIT,			//login监测编辑框
	ET_HOT_LIST,			//获取热门推荐
	ET_CHECK_LOGIN,			//检查登录
	ET_RELOAD_ERROR,		//错误恢复定时器
	ET_CHECKNET,			//检查网络定时器			
	ET_AUTOLOGIN,			//自动登陆滞后处理
	ET_FRESHTOKEN,			//TOKEN刷新失败检查
	ET_SDKLOGIN,			//SDK同步登录状态
	ET_MAXINIT,				//初始化最大化显示	
	ET_SHOWWEBMSG			//显示跳转第三方播放信息
};

enum MyMsg
{
	UM_BEGIN_MSG = WM_USER + 10,
	UM_FLASH_PLAY_END,//播放结束
	UM_LIST_RBUTTONDOWN,
	UM_LAST_NEXT_ENABLE,//上下一个启用禁用
	UM_RECHECK_TOP,//检测窗口置顶属性
	UM_SETTING_PAGE,
	UM_UPDATE_SKIN,//更新播放顺序按钮背景
	//UM_ENABLE_STOP,//启用禁用控制栏按钮
	UM_WEB_MSG,//web控件相关消息  wparam: 0=>控件是否显示(lparam：0，1)
	UM_REDARW,
	UM_SET_PLAY,
	UM_PLAYINFO,//播放详情信息
	UM_SETIMAGE,//设置图片    
	UM_DEL_SEARCHLIST,
	UM_SEARCHLIST,
	UM_WNDBTN_DEL,	
	UM_SHOW_LISTWND,//是否显示列表窗口 lParam==0 不显示 1 显示
	UM_REDRAW_LISTWND,//重绘列表窗
	UM_SETMIMI_WND,//迷你模式窗口
	UM_MIGUI_HIDE,
	UM_SEARCH_HELP,
	UM_EXT_SET,
	UM_CLOSE,
	UM_CLEAR_LIST,//清空播放记录
	UM_REDRAW_SEARCHLIST,//重绘搜索列表
	UM_HTTP_LOGIN,
	UM_HTTP_REGISTER,
	UM_HTTP_RESETPSWD,
	UM_NO_EDITCHANGE,//编辑框内容改变，不更新搜索
	UM_PLAY_LAST,//播放上一集
	UM_PLAY_NEXT,//播放下一集
	UM_PLAY_PAUSE,//暂停/继续
	UM_PLAY_STOP,//停止
	UM_PLAY_POS,//播放进度
	UM_SET_PLAYPOS,//
	UM_BTN_SET,//调起小设置
	UM_SET_FULLSCREEN,//设置全屏
	UM_MINI_MODE,
	UM_SKIN_FAVORITE,
	UM_SHOW_VOLUME,
	UM_VLUME_POS,
	UM_VIP,
	UM_HIDE_RESOLU,
	UM_PLAY_ERROR,
	UM_MINI_CLOSE,
	UM_MINI_MIN,
	UM_MINI_MAX,
	UM_RESETWND,
	UM_SHOW_PLAYHEAD_MSG,
	UM_SHOW_BIND_PHONE,
	UM_LOGIN_CLOSE,
	UM_REDRAW_LISTWND_NOSORT,//不排序刷新列表播放时间
	UM_PLAY,
	UM_DETAIL_FUNTV,
	UM_PLAY_FUNTV,
	UM_DETAIL_KANKAN,
	UM_WEB_PLAY,
	UM_PLAY_KANKAN,
	UM_SHOWMAIN,
	UM_AI_PLAY,
	UM_AI_PAUSE,
	UM_STOP
};


enum CMD_ID
{
	CMD_About = 1,              //关于联想
	CMD_Update,                 //在线升级
	CMD_Feedback,               //意见反馈
	CMD_Setting,                //设置
	CMD_QUIT,                   //退出
	OPEN_FILE = 10,             //打开文件
	OPEN_FLODER,                //打开文件夹
	OPEN_URL,                   //打开url
	OPEN_DOWNLOAD_URL,          //打开下载链接
	CMD_DEL,                    //删除
	CMD_DOWNLOAD,               //下载
	CMD_DELALL,                 //删除全部
	CMD_OPENFLODER,             //打开文件位置
	NO_PLAY_RECORD,             //无痕播放
	REMOVE_NOPLAY_RECORD,       //取消无痕播放
	CMD_PLAY = 20,              //播放
	CMD_PAUSE,                  //暂停/播放
	CMD_STOP,                   //停止
	CMD_FORWARD,                //快进
	CMD_REWIND,                 //快退
	CMD_ACCELERATION,           //加速
	CMD_DECELERATION,           //减速
	CMD_NORMALRATION,            //正常速度
	CMD_LAST_FRAME,             //上一帧
	CMD_NEXT_FRAME,             //下一帧
	CMD_HEAD_END,               //跳过/取消跳过片头片尾
	CMD_SET_A,                  //设置A点
	CMD_SET_B,                  //设置B点
	CMD_NO_AB,                  //取消AB点
	FULL_SCREEN = 40,           //全屏
	MINI_SCREEN,                //mini屏
	NORMAL_SCREEN,              //正常
	SCREEN_NORMAL,              //原始比例
	SCREEN_16_9,
	SCREEN_4_3,
	SCREEN_FULL,                //铺满

	SKIP_HEAD_END = 49,           //跳过片头片尾
	NOSKIP_HEAD_END,            //取消跳过片头片尾

	AudioChannel_Stereo = 60,       //原始声道
	AudioChannel_Left,              //左声道
	AudioChannel_Right,             //右声道
	AudioChannel_RStereo,           //反转立体声道   
	AudioChannel_Dolbys,            //左右环绕声道   

	CMD_MUTE = 54,                   //静音

	SUBTITLE_LOAD,              //加载字幕
	SUBTITLE_SEC,               //次字幕
	SUBTITLE_SET,               //设置字幕
	SUBTITLE_CHILDS,            //字幕子轨迹

	Sequential_play = 80,       //顺序播放
	Random_play,                //随机播放
	Single_play,                //单个播放
	Single_cycle,               //单个循环
	List_loop,                  //列表循环

	NOTOPMOST = 95,             //从不置顶
	ALWAYS_TOPMOST,             //始终置顶
	SUBTITLE_START = 100,//字幕开始ID，默认为取消
};

enum PlayStyle
{
	Sequential,
	Random,
	Single,
	SingleCycle,
	ListCycle
};

enum _HotkeyID
{
	HK_OPENFILE = 1500,
	HK_OPENFLODER,
	HK_OPENURL,
	HK_PAUSE,
	HK_STOP,
	HK_QUIT,
	HK_FORWARD,
	HK_REWIND,
	HK_FORWARD30,
	HK_REWIND30,
	HK_UPVOICE,
	HK_DOWNVOICE,
	HK_MUTE,
	HK_FULLSCREEN,
};

enum ePlayerWndMode
{
	ePlayerWndMode_Init,
	ePlayerWndMode_Nomal,
	ePlayerWndMode_Mini,
	ePlayerWndMode_Full
};

enum eLocalFileType
{
	eLocalFile_Default,
	eLocalFile_swf
};

enum Web_wParam
{
	WP_WEB_SHOW,
};


#define BOOTRUN_KEY_NAME _T("LMPlayer")
#define LENOVO_APP_TOKEN _T("1C6RWC3KM391")
#define LENVOV_REALM "leplayer.stv.lenovo.com"
#define LENOVO_SOURCE_W L"com.lenovopc.rocket.clientlogin"
#define LENOVO_SOURCE_A "com.lenovopc.rocket.clientlogin"
#define MIGU_HTTP_IP "https://www.miguvideo.com"//L"http://117.131.17.174:8083"

#define UM_COPYDATA_CMDLING			10078
#define UM_COPYDATA_YOUKU			6
#define SHARE_MEMORY_SIZE			2*1024
#define LMPLAYER_MUTEX_STRING	  L"{A974FD90-34FE-4B69-A509-75E43255FE49}"
#define LEUPDATE_MUTEX_STRING	  L"{95209F0E-6D08-48BE-B660-5161524B371F}"

#define WIDTH_PLAYLIST			220
#define WIDTH_LEFTFRAME			1
#define WIDTH_SPLITER			1
#define WIDTH_RIGHTFRAME		1
#define HEIGHT_TOPFRAME			0
#define HEIGHT_SPLITER			0
#define HEIGHT_BOTTOMFRAME		1
#define HEIGHT_TITLE			30

#define HEIGHT_CTRBAR			46
#define WIDTH_PLAYER_MINI		500
#define HEIGHT_PLAYER_MINI		280

#define RECORD_SHOW				5

#define REALM "leplayer.stv.lenovo.com"
#define REALMGP "pcassistant.lenovo.com"