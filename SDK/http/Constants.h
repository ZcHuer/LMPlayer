#ifndef CONSTANTS_H_INCLUDED
# define CONSTANTS_H_INCLUDED

#include "Common.h"

namespace LeConnect
{
	// LV smaller is higher
	enum Priority
	{
		LV0 = 0, LV1 = 1,
	};
	static const INT PRIORITY_LEN = 2;
	
	static Priority Priority_valueOf(INT i)
	{
		switch(i)
		{
		case 0:return LV0;
		case 1:return LV1;
		}
		return LV0;
	}

	interface Constants
	{
		static const INT DATABASE_VERSION = 7;
		static const INT EVENT_NUMBER_PER_GETTING = 300;
		static const INT MAX_PERMISSION_NUMBER = 20;
	};

	interface StorageQueue 
	{
		static const INT PRIO_LEN = 2;
		static const INT PRIO_EVENT_NUM[];
		static const Priority PRIORITY_DEFAULT = LV0;
	};

	interface Http {
		static LPCWSTR HTTP_HEADER_USER_AGENT;
		static const STRING HTTP_HEADER_HOST;
	};

	interface SharedPreference {
		static const STRING MSG_FILENAME;
	};

	interface ServerConfig 
	{
		static const STRING SIGN_IGNORE;
		static const STRING VALUE_SEP;
		static const STRING JSON_SEP;
		interface AddSessionIntervalConfig
		{
			static const INT SECOND_DEFAULT = 30;
			static const STRING CATEGORY;
		};

		interface UpdateIntervalConst 
		{
			static const INT INTERVAL_MINUTE_DEFAULT = 360;
			static const STRING CATEGORY;
			static const STRING KEY_TIMESTAMP;
		};

		interface DeviceSendConst 
		{
			static const STRING CATEGORY;
			static const BOOL DEFAULT = true;
		};

		interface EventNotSendConst 
		{
			static const STRING CATEGORY_SENDFLAG;
			static const BOOL DEFAULT = true;
		};

		interface DispatchConst 
		{
			static const INT STATUS_LEN = 2;
			static const INT REPORT_LEN = 3;
			static const STRING CATEGORY_DISPATCH_2G;
			static const STRING CATEGORY_DISPATCH_3G4G;
			static const STRING CATEGORY_DISPATCH_Wifi;
			static const INT STATUS_3G4G[ServerConfig::DispatchConst::STATUS_LEN];
			static const INT STATUS_2G[ServerConfig::DispatchConst::STATUS_LEN];
			static const INT STATUS_WIFI[ServerConfig::DispatchConst::STATUS_LEN];
			static const BOOL REPORT_Wifi[ServerConfig::DispatchConst::REPORT_LEN];
			static const BOOL REPORT_3G4G[ServerConfig::DispatchConst::REPORT_LEN];
			static const BOOL REPORT_2G[ServerConfig::DispatchConst::REPORT_LEN];
		};

		interface CompressConst 
		{
			static const INT STATUS_3G4G = 3;
			static const INT STATUS_2G = 3;
			static const STRING CATEGORY_COMPRESS_2G;
			static const STRING CATEGORY_COMPRESS_3G4G;
		};

		interface HttpTimeoutConst 
		{
			static const STRING CATEGORY;
			static const INT SECOND_DEFAULT = 20;
		};

		interface EventPriorityConst 
		{
			static const STRING CATEGORY;
			static const Priority DEFAULT = LeConnect::StorageQueue::PRIORITY_DEFAULT;
		};

		interface SessionTimeEnableConst 
		{
			static const STRING CATEGORY;
			static const BOOL DEFAULT = false;
		};

		interface MsgConfigConst 
		{
			static const STRING CATEGORY_ENABLE;
			static const STRING CATEGORY_ELAPSED_MINUTES;
			static const BOOL ENABLE_DEFAULT = true;
			static const INT NEED_ELAPSED_MINUTES_DEFAULT = 20;
		};
	}; //end interface ServerConfig

	interface Network
	{
		static const INT STATUS_NOTCONCERN = 0; // status lan
		static const INT STATUS_OFFLINE = 1;
		static const INT STATUS_WIFI = 2;
		static const INT STATUS_3G4G = 3;
		static const INT STATUS_2G = 4;
	};

	enum InitalizeMode 
	{
		Smart = 0,
		Normal = 1,
	};
}
#endif // CONSTANTS_H_INCLUDED