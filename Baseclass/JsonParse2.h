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
�����������������ȡjson��������������UTF8�����ַ���
�ڲ�����c++�쳣
*/
BOOL ReadJson(INetStream* lpStream, Json::Value& root);

/*
������������������Json������������UTF8�����ַ���
������lplError ʱ, ��"code"���ֵ�����*lplError�У�������ֵ�Ƿ����DEF_ERRORCODE_OK�� ���ڷ���TRUE,���򷵻�FALSE
������lpstrErrMsg ʱ���� "res\remindMsg"���ֵ�����*lpstrErrMsg��
�ڲ�����c++�쳣
*/
BOOL ReadJson(INetStream* lpStream, Json::Value& root, LONG* lplError, CString* lpstrErrMsg);


void ConvertJsonArrayToVector(Json::Value& array, std::vector<ULONGLONG>& vec);
void ConvertVectorToJsonArray(const std::vector<ULONGLONG>& vec, Json::Value& array);


