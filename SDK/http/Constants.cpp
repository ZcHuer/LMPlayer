#include "Constants.h"

namespace LeConnect
{
	LPCWSTR Http::HTTP_HEADER_USER_AGENT = L"WinHTTP LePlayer/2.0.2.16";
	const STRING Http::HTTP_HEADER_HOST = _T("Host");
	
	const INT StorageQueue::PRIO_EVENT_NUM[StorageQueue::PRIO_LEN] = {600, 600};

	const STRING SharedPreference::MSG_FILENAME = _T("Msg");

	const STRING ServerConfig::SIGN_IGNORE = _T("#Ignore#");
	const STRING ServerConfig::VALUE_SEP = _T("\x1");
	const STRING ServerConfig::JSON_SEP = _T("\x1");
	const STRING ServerConfig::AddSessionIntervalConfig::CATEGORY = _T("AddSessionInterval");
	const STRING ServerConfig::UpdateIntervalConst::CATEGORY = _T("UpdateInterval");
	const STRING ServerConfig::UpdateIntervalConst::KEY_TIMESTAMP = _T("LastUpdateTimestamp");
	const STRING ServerConfig::DeviceSendConst::CATEGORY = _T("SDKEnable");
	const STRING ServerConfig::EventNotSendConst::CATEGORY_SENDFLAG = _T("EventNotSend");
	const STRING ServerConfig::DispatchConst::CATEGORY_DISPATCH_2G = _T("Dispatch2G");
	const STRING ServerConfig::DispatchConst::CATEGORY_DISPATCH_3G4G = _T("Dispatch3G4G");
	const STRING ServerConfig::DispatchConst::CATEGORY_DISPATCH_Wifi = _T("DispatchWifi");
	const INT ServerConfig::DispatchConst::STATUS_3G4G[ServerConfig::DispatchConst::STATUS_LEN] = {1, 20};
	const INT ServerConfig::DispatchConst::STATUS_2G[ServerConfig::DispatchConst::STATUS_LEN] = {1, 20};
	const INT ServerConfig::DispatchConst::STATUS_WIFI[ServerConfig::DispatchConst::STATUS_LEN] = {1, 1};
	const BOOL ServerConfig::DispatchConst::REPORT_Wifi[ServerConfig::DispatchConst::REPORT_LEN] = { true, true, true };
	const BOOL ServerConfig::DispatchConst::REPORT_3G4G[ServerConfig::DispatchConst::REPORT_LEN] = { true, true, true };
	const BOOL ServerConfig::DispatchConst::REPORT_2G[ServerConfig::DispatchConst::REPORT_LEN] = { true, true, true };
	const STRING ServerConfig::CompressConst::CATEGORY_COMPRESS_2G = _T("Compress2G");
	const STRING ServerConfig::CompressConst::CATEGORY_COMPRESS_3G4G = _T("Compress3G4G");
	const STRING ServerConfig::HttpTimeoutConst::CATEGORY = _T("HttpTimeout");
	const STRING ServerConfig::EventPriorityConst::CATEGORY = _T("EventPriority");
	const STRING ServerConfig::SessionTimeEnableConst::CATEGORY = _T("SessionTimeEnable");
	const STRING ServerConfig::MsgConfigConst::CATEGORY_ENABLE = _T("MsgEnable");
	const STRING ServerConfig::MsgConfigConst::CATEGORY_ELAPSED_MINUTES = _T("MsgNeedElapsed");

}