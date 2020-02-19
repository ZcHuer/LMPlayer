/************************************************************************//*
创建：		2019.1.10
作者：		刘会山
功能：		统计模块-离线数据
			enum类型，为每个相同类预留数字区间，区间内，只能递增，默认+1，设定上线后不能修改! 如果超过了区间，另外再加区间。	 
统计分类：
			一、统计类	0	 - 999
			二、选择类	1000 - 1999
			三、页面统计	2000 - 2999
*//************************************************************************/

#pragma once
enum eReportData
{
	/*	一、统计类，计数，数据ID范围:0-9999,其中：客户端0-4999，页面5000-9999 */
	/*	客户端 0 - 4999	*/

	// 程序相关，安装启动退出卸载等
	ERD_COUNT_RUN_TOTAL					= 0,	// 启动-总数
	ERD_COUNT_RUN_DESKTOPLINK			= 1,	// 启动-桌面快捷方式
	ERD_COUNT_RUN_STARTMENU				= 2,	// 启动-开始菜单	
	ERD_COUNT_RUN_FILEASS				= 3,	// 启动-文件关联
	ERD_COUNT_RUN_PCM					= 4,	// 启动-悬浮球
	ERD_COUNT_RUN_LEWIN					= 5,	// 启动-乐窗
	//ERD_COUNT_RUN_CMDLINE				= 6,	// 启动-命令行  所有启动都为命令行，暂时无此需求，保留
	//ERD_COUNT_RUN_POPWIN				= 7,	// 启动-弹窗调起  暂时不能识别
	ERD_COUNT_RUN_BROWSER				= 8,	// 启动-浏览器调起	 暂时没有
	ERD_COUNT_QUIT						= 9,	// 启动-退出总数
	ERD_COUNT_QUIT_TROY					= 10,	// 启动-托盘退出
 // ERD_COUNT_RUN_TASKBAR				= 11,	// 启动-任务栏 暂时不能识别这个启动了，因为用户手动锁定的无标识
	// 鼠标点击
	ERD_COUNT_CLICK_FIND				= 20,	// 点击-找片 tab
	ERD_COUNT_CLICK_PLAY				= 21,	// 点击-播放 tab
	ERD_COUNT_CLICK_FAXIAN				= 22,	// 点击-发现 tab
	ERD_COUNT_CLICK_MAX					= 23,	// 点击-最大化
	ERD_COUNT_CLICK_MIN					= 24,	// 点击-最小化
	ERD_COUNT_CLICK_CLOSE				= 25,	// 点击-关闭
	ERD_COUNT_CLICK_HIDERIGHT			= 26,	// 点击-隐藏右侧栏
	ERD_COUNT_CLICK_SHOWRIGHT			= 27,	// 点击-显示右侧栏	
	ERD_COUNT_CLICK_SEARCH				= 28,	// 点击-搜索按钮
	ERD_COUNT_CLICK_JILU				= 29,	// 点击-记录
	ERD_COUNT_SHOW_JILU					= 30,	// 鼠标滑过/显示
	ERD_COUNT_CLICK_HISTORY				= 31,	// 点击-看过，点击就是显示
	ERD_COUNT_SHOW_HISTORY_MORE			= 32,	// 点击-看过-查看更多
	ERD_COUNT_CLICK_COLLECT				= 33,	// 点击-收藏，点击就是显示
	ERD_COUNT_SHOW_COLLECT_MORE			= 34,	// 点击-收藏-查看更多
	ERD_COUNT_CLICK_REFRESH				= 35,	// 点击-刷新
	ERD_COUNT_CLICK_MENU				= 50,	// 菜单 - 点击	
	ERD_COUNT_CLICK_MENU_FILE			= 51,	// 菜单 - 播放本地文件按钮点击
	ERD_COUNT_CLICK_MENU_ABOUT			= 52,	// 菜单 - 关于LMPlayer
	ERD_COUNT_CLICK_MENU_UPDATE			= 53,	// 菜单 - 在线升级按钮点击
	ERD_COUNT_CLICK_MENU_FEEDBACK		= 54,	// 菜单 - 意见反馈
	ERD_COUNT_CLICK_MENU_SET			= 55,	// 菜单 - 设置
	ERD_COUNT_CLICK_MENU_LOGOUT			= 56,	// 菜单 - 退出登录
	ERD_COUNT_CLICK_MENU_QUIT			= 57,	// 菜单 - 退出
	ERD_COUNT_SHOW_SEARCH				= 58,	// 显示 - 搜索框
	ERD_COUNT_CLICK_NEXT				= 59,	// 点击 - 下一集
	ERD_COUNT_CLICK_VOLUME				= 60,	// 点击 - 音量图标
	//									= 61,	
	ERD_COUNT_CLICK_TUOPAN				= 62,	// 点击 - 托盘
	ERD_COUNT_CLICK_ZHIDING_CONGBU		= 63,	// 点击 - 从不置顶
	ERD_COUNT_CLICK_ZHIDING_SHIZHONG	= 64,	// 点击 - 始终置顶
	ERD_COUNT_CLICK_ZHIDING_PLAY		= 65,	// 点击 - 播放时置顶
	ERD_COUNT_CLICK_FULL				= 66,	// 点击 - 全屏总数
	ERD_COUNT_CLICK_FULL_YOUJIAN		= 67,	// 点击 - 右键菜单全屏
	ERD_COUNT_CLICK_FULL_KONGZHILAN		= 68,	// 点击 - 控制栏全屏
	ERD_COUNT_CLICK_FULL_HOTKEY			= 69,	// 点击 - 快捷键全屏
	ERD_COUNT_CLICK_FULL_DOUBLECLICK	= 70,	// 点击 - 双击全屏
	ERD_COUNT_CLICK_MINI				= 71,	// 点击 - mini模式总数
	ERD_COUNT_CLICK_MINI_YOUJIAN		= 72,	// 点击 - 右键mini模式
	ERD_COUNT_CLICK_MINI_CTRLBAR		= 73,	// 点击 - 控制栏
	ERD_COUNT_CLICK_CTRL_OPENFILE		= 74,	// 控制栏-本地-打开文件
	ERD_COUNT_CLICK_CTRL_SET			= 75,	// 控制栏-本地-设置
				
	// 播放相关，在线播放本地播放等，包括点击按钮
	ERD_COUNT_PLAY_OPENFILE_LOGO		= 100,	// 打开文件-播放窗的主按钮
	ERD_COUNT_PLAY_OPENFILE_LIST		= 101,	// 打开文件-播放窗的主按钮的下拉三角
	ERD_COUNT_PLAY_OPENFILE				= 102,	// 下拉三角菜单种的打开文件
	ERD_COUNT_PLAY_OPENFILEFLODER		= 103,	// 下拉三角菜单种的打开文件夹
	ERD_COUNT_PLAY_TOTAL				= 104,	// 播放总数
	ERD_COUNT_PLAY_FILESUCCESS			= 105,	// 播放文件成功
	ERD_COUNT_PLAY_FILEFAILED			= 106,	// 播放文件失败
	ERD_COUNT_PLAY_NETSUCCESS			= 107,	// 播放流成功
	ERD_COUNT_PLAY_NETFAILED			= 108,	// 播放流失败	
	ERD_COUNT_PLAY_PLAY					= 109,	// 播放-总数，暂时无法区分来源，下个版本再细做
	ERD_COUNT_PLAY_PAUSE				= 110,	// 暂停-总数，暂时无法区分来源，下个版本再细做
	ERD_COUNT_PLAY_FORWORD				= 111,	// 快进
	ERD_COUNT_PLAY_REWIND				= 112,	// 快退

	// 播放列表操作
	ERD_COUNT_FILELIST_ADD				= 130,  // 添加按钮
	ERD_COUNT_FILELIST_OPENFILE			= 131,  // 打开文件
	ERD_COUNT_FILELIST_OPENFLODER		= 132,	// 打开文件夹	
	ERD_COUNT_FILELIST_SHUNXU			= 133,	// 顺序播放
	ERD_COUNT_FILELIST_SUIJI			= 134,	// 随机播放
	ERD_COUNT_FILELIST_SINGLE			= 135,	// 单个播放
	ERD_COUNT_FILELIST_SINGLEXUNHUAN	= 136,	// 单个循环
	ERD_COUNT_FILELIST_LISTXUNHUAN		= 137,	// 列表循环
	ERD_COUNT_FILELIST_CLEAR			= 138,	// 清空按钮
	
	// 个人中心，登陆注册等	
	ERD_COUNT_REGISTER_GETCODE			= 160,	// 注册-获取手机验证码	
	ERD_COUNT_REGISTER_REGISTER			= 161,	// 注册-完成
	ERD_COUNT_LOGIN_LOGINBYPSWD			= 162,	// 账号密码 - 点击登录	
	ERD_COUNT_LOGIN_LOGINBYPHONE		= 163,	// 手机快捷登录	
	ERD_COUNT_LOGIN_FORGETPSWD			= 164,	// 忘记密码	
	ERD_COUNT_LOGIN_GETPSWD_REMEBER		= 165,	// 勾选记住密码
	ERD_COUNT_LOGIN_SETAUTOLOGIN		= 166,	// 勾选自动登录
	ERD_COUNT_LOGIN_BIND_GETCODE		= 167,	// 绑定手机号 - 获取验证码
	ERD_COUNT_LOGIN_BIND_LOGIN			= 168,	// 绑定手机号 - 绑定并登录
	ERD_COUNT_LOGIN_PHOTO_CLICK			= 169,	// 点击头像
	ERD_COUNT_LOGIN_LOGIN_SUCCESS		= 170,	// 登陆成功
	ERD_COUNT_LOGIN_SWITCH				= 171,	// 切换账号
	ERD_COUNT_LOGIN_LOGOUT				= 172,	// 退出登陆
	ERD_COUNT_LOGIN_CLOSE_REGISTER		= 173,	// 注册-关闭
	ERD_COUNT_LOGIN_CLOSE_PSWD			= 174,	// 账号密码 - 关闭
	ERD_COUNT_LOGIN_CLOSE_PHONE			= 175,	// 手机快捷登录-关闭
	ERD_COUNT_LOGIN_CLOSE_GETPSWD		= 176,	// 密码找回 - 关闭
	ERD_COUNT_LOGIN_CLOSE_BIND			= 177,	// 绑定手机号 - 关闭
	ERD_COUNT_LOGIN_GETPSWD_GETCODE		= 178,	// 找回密码 - 获取验证码
	ERD_COUNT_LOGIN_BIND_SUCCESS		= 179,	// 绑定手机号 - 绑定成功
		
	// 升级相关 200	-	249
	// vip相关 250	-	299
	ERD_COUNT_VIP_CLICKBTN				= 250,	// 点击-VIP按钮
	ERD_COUNT_VIP_SHOWWINDOW			= 251,	// 显示-VIP窗口
	ERD_COUNT_VIP_BUY					= 252,	// 点击-VIP开通
	ERD_COUNT_VIP_BUYAGIN				= 253,	// 点击-VIP续费

	// 鼠标点击-设置
	ERD_COUNT_SET_QUIT					= 300,	// 设置 - 关闭	
	ERD_COUNT_SET_SAVE					= 301,	// 设置 - 保存
	ERD_COUNT_SET_CENCEL				= 302,	// 设置 - 取消
	ERD_COUNT_SET_DEFAULT				= 303,	// 设置 - 【隐私设置】恢复默认设置
	ERD_COUNT_SET_CLEARHISTROY			= 304,	// 设置 - 【隐私设置】立即清除隐私记录
	// 其他扩展					300	-	399
	// 格式统计					500	-	999
	/*	页面 5000 - 9999	 */
	// 具体数据项由页面维护

	/*	二、选择类，0或1，数据ID范围:10000-19999,其中：客户端10000-14999，页面14999-19999 */
	/*	客户端 10000-14999	*/
	ERD_BOOL_SET_AUTORUN				= 10000, //	设置 - 【常规】开机自启播放器勾选
	ERD_BOOL_SET_CLOSEMIN				= 10001, // 设置 - 【常规】关闭时 - 最小化至托盘
	ERD_BOOL_SET_CLOSEQUIT				= 10002, // 设置 - 【常规】关闭时 - 退出程序
	ERD_BOOL_SET_CLOSECLEAR				= 10003, // 设置 - 【隐私设置】退出时清空播放记录
	ERD_BOOL_SET_RUNCLEAR				= 10004, // 设置 - 【隐私设置】开启无痕浏览模式	
	ERD_BOOL_SET_AUTOUPDATE				= 10005, // 设置 - 【升级与更新】自动更新版本
	ERD_BOOL_SET_UPDATETIP				= 10006, // 设置 - 【升级预更新】有更新时提醒
	
	/*	页面 14999-19999	*/
	// 具体数据项由页面维护	

	/*	三、字符串类，后边接字符串，数据ID范围:20000-29999,其中：客户端20000-24999，页面24999-29999 */
	/*	客户端 10000-14999	*/
	/*	页面 14999-19999	*/
	// 具体数据项由页面维护	
};
