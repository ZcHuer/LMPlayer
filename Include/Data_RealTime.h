/************************************************************************//*
创建：	2019.1.11
作者：	刘会山
功能：	统计模块-实时数据
		兼容原有设计，把数据项做了宏定义,方便集中管理和检索
*//************************************************************************/

//	eventID库

#define RTD_PLAY				"1"   // 播放
#define	RTD_PAUSE				"2"   // 暂停
#define	RTD_FFORWARD			"3"   // 快进
#define	RTD_REWIND				"4"   // 快退
#define	RTD_SEARCH				"5"   // 搜索
#define	RTD_REFRESH				"6"   // 刷新
#define	RTD_SELECT				"7"   // 选集
#define	RTD_DOWNLOAD			"8"   // 下载
#define	RTD_SHARE				"9"   // 分享
#define	RTD_COLLECTION			"10"   // 收藏 / 取消收藏（用eventDes区分）
#define	RTD_COMMENT				"11"   // 评论
#define	RTD_BARRAGE				"12"   // 弹幕
#define	RTD_CACHE				"13"   // 缓存
#define	RTD_SWITCH				"14"   // 切换 清晰度
#define	RTD_THUMBSUP			"15"   // 点赞
#define	RTD_LOGIN				"16"   // 登陆
#define	RTD_REGIST				"17"   // 注册
#define	RTD_ADDINFO				"18"   // 完善信息
#define	RTD_RUNPLAYER			"19"   // 启动播放器
#define	RTD_CLOSEPLAYER			"20"   // 关闭播放器
#define RTD_CANCELFULL			"23"   //取消全屏
#define	RTD_CHANGE				"25"   // 换一批
#define	RTD_MORE				"26"   // 更多
#define	RTD_LOGOUT				"27"   // 注销
#define	RTD_FEEDBACK			"28"   // 问题反馈 / 我要吐槽
#define	RTD_FULLSCREEN			"30"   // 投屏 / 置顶 / 全屏 = full / 播放模式(顺序、循环) = model / 清空列表 = clear / 最大化 / 迷你 = mini / 最小化 / 托盘 / 进度条拖拽 = drag（用eventDes区分）
#define	RTD_BUYVIP				"31"   // 购买vip(立即开通会员)
#define	RTD_MOUSECOVER			"32"   // "PC客户端鼠标悬停   个人中心/收藏记录/播放记录，用eventDes区分个人中心 = center / 收藏记录 = collect / 播放记录 = play"
#define RTD_MAXBTN				"35"   //点击最大化，还原按钮
#define	RTD_ADDFILE				"36"   // 本地视频添加
#define	RTD_GETEMS				"39"    // 获取短信验证码
#define	RTD_PAGEVIEW			"40"   // 页面浏览
#define	RTD_CLICK				"41"   // 点击
#define	RTD_CONTINUE			"42"   // 断点续播
#define	RTD_START				"43"   // 开机
#define	RTD_SHUTDOWN			"44"   // 关机
#define	RTD_PAYFOR				"45"   // 支付
#define	RTD_CLOSEAPP			"46"   // 关闭应用
#define	RTD_RUNAPP				"47"   // 启动应用
#define	RTD_APPOINTMENT			"49"   // 预约
#define	RTD_HOTSEARCH			"50"   // 热门搜索
#define	RTD_RENEW				"51"   // 立即续费
#define	RTD_ACTIVEADNINSTALL	"52"   // 激活，安装
#define	RTD_UNINSTALL			"53"   // 卸载
#define	RTD_UPDATE				"54"   // 更新 / 升级
#define	RTD_BINDPHONE			"55"   // 绑定手机号
#define	RTD_SAVESHUTCUT			"56"   // 保存快捷方式到桌面
#define	RTD_HID					"57"   // 收起 / 展开(用eventDes区分)
#define	RTD_SET					"58"   // 设置
#define	RTD_RETRIEVEPSWD		"59"   // 找回密码
#define	RTD_POP					"60"   // 弹出
#define	RTD_STOP				"61"   // 停止
#define	RTD_LUCKDRAW			"62"   // 抽奖
#define	RTD_SIGNIN				"63"   // 签到
#define	RTD_RUNOTHERAPP			"64"   // 启动本地应用(第3方)
#define	RTD_CLOSEPOP			"65"   // 关闭广告 / 弹窗