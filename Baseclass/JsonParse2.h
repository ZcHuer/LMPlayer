#pragma once
#include "JsonParse.h"
#include <vector>

LONG GetLong(Json::Value& root, LPCSTR lpsKey, LONG defaultValue = 0);
LONGLONG GetLongLong(Json::Value& root, LPCSTR lpsKey, LONGLONG defaultValue = 0);

LONGLONG GetLongLong(Json::Value& root, LONGLONG defaultValue = 0);
LONG GetLong(Json::Value& root, LONG defaultValue = 0);
BOOL GetJsonString(Json::Value& root, CString& strValue);


#define DEF_ERRORCODE_OK				1

/*
从网络数据流对象读取json串，此流必须是UTF8编码字符串
内部捕获c++异常
*/
BOOL ReadJson(INetStream* lpStream, Json::Value& root);

/*
从网络流缓冲区解析Json，此流必须是UTF8编码字符串
当存在lplError 时, 把"code"结点值存放在*lplError中，并检测该值是否等于DEF_ERRORCODE_OK， 等于返回TRUE,否则返回FALSE
当存在lpstrErrMsg 时，把 "res\remindMsg"结点值存放在*lpstrErrMsg中
内部捕获c++异常
*/
BOOL ReadJson(INetStream* lpStream, Json::Value& root, LONG* lplError, CString* lpstrErrMsg);


void ConvertJsonArrayToVector(Json::Value& array, std::vector<ULONGLONG>& vec);
void ConvertVectorToJsonArray(const std::vector<ULONGLONG>& vec, Json::Value& array);


