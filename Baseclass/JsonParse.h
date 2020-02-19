#pragma once
#include "..\3rdparty\jsoncpp\include\json\json.h"

/*
���ļ���ȡJson����
�ļ������� UTF8 ��BOMͷ��ʽ����
�ļ����ݱ���С��5M�����򷵻ش���
�ڲ�����c++�쳣
*/
BOOL ReadJsonFile(LPCWSTR lpwsJsonFile, Json::Value& root);

/*
����UTF16�����Json��ת��ΪUTF8�����ٽ���
�ڲ�����c++�쳣
*/
BOOL ReadJson(LPCWSTR lpwsJson, Json::Value& root);

/*
��ȡ��ǰ���Ķ�Ӧ����ֵ
ע����ȡ��������ֵʱ���ڲ������ַ������ֵ�ת��
�ڲ�����c++�쳣
*/
BOOL GetJsonStringUTF8(Json::Value& node, CStringA& strValue);
BOOL GetJsonStringUTF16(Json::Value& node, CString& strValue);	//�ڲ���UTF8��UTF16ת��
BOOL GetJsonLong(Json::Value& node, LONG& lValue);
BOOL GetJsonULong(Json::Value& node, ULONG& lValue);
BOOL GetJsonLongLong(Json::Value& node, LONGLONG& llValue);
BOOL GetJsonULongLong(Json::Value& node, ULONGLONG& llValue);
BOOL GetJsonBool(Json::Value& node, BOOL& bValue);//�ڲ������ַ���true/false, ����0���0

/*
��ȡ��ǰ�ַ������ͽ��Ķ�Ӧ��������ֵ
�ڲ������ַ������ֵ�ת��
�ڲ�����c++�쳣
*/
BOOL GetJsonLongFromString(Json::Value& node, LONG& lValue);
BOOL GetJsonULongFromString(Json::Value& node, ULONG& lValue);
BOOL GetJsonLongLongFromString(Json::Value& node, LONGLONG& llValue);
BOOL GetJsonULongLongFromString(Json::Value& node, ULONGLONG& llValue);
BOOL GetJsonBoolFromString(Json::Value& node, BOOL& bValue);

/*
��ȡ��ǰ���������ӽ���Ӧ����ֵ
֧�ֶ༶�ӽ��,�磺 subnode1\subnode2\subnode3
��ȡ��������ֵʱ���ڲ������ַ������ֵ�ת��
�ڲ�����c++�쳣
*/
BOOL GetJsonStringUTF8(Json::Value& node, LPCSTR lpsKey, CStringA& strValue);
BOOL GetJsonStringUTF16(Json::Value& node, LPCSTR lpsKey, CString& strValue);
BOOL GetJsonLong(Json::Value& node, LPCSTR lpsKey, LONG& lValue);
BOOL GetJsonULong(Json::Value& node, LPCSTR lpsKey, ULONG& lValue);
BOOL GetJsonLongLong(Json::Value& node, LPCSTR lpsKey, LONGLONG& llValue);
BOOL GetJsonULongLong(Json::Value& node, LPCSTR lpsKey, ULONGLONG& llValue);
BOOL GetJsonBool(Json::Value& node, LPCSTR lpsKey, BOOL& bValue);

/*
��ȡ��ǰ�ַ������ͽ��������ӽ��Ķ�Ӧ��������ֵ
֧�ֶ༶�ӽ��,�磺 subnode1\subnode2\subnode3
�ڲ������ַ������ֵ�ת��
�ڲ�����c++�쳣
*/
BOOL GetJsonLongFromString(Json::Value& node, LPCSTR lpsKey, LONG& lValue);
BOOL GetJsonULongFromString(Json::Value& node, LPCSTR lpsKey, ULONG& lValue);
BOOL GetJsonLongLongFromString(Json::Value& node, LPCSTR lpsKey, LONGLONG& llValue);
BOOL GetJsonULongLongFromString(Json::Value& node, LPCSTR lpsKey, ULONGLONG& llValue);

/*
��ȡ�ӽ�㣬֧�ֶ༶�ӽ��
�ڲ�����c++�쳣
�磺 subnode1\subnode2\subnode3
*/
BOOL GetJsonNode(const Json::Value& root, LPCSTR lpsPath, Json::Value& node);

/*
��ȡ�ӽ�㣬֧�ֶ༶�ӽ�㣬֧�������ӽ��
�ڲ�����c++�쳣
XPath ʾ��:
nodename
[index]
nodename1\nodename2\nodename3
[index]\nodename1\nodename2
nodename1\nodename2[index]\nodename3
*/
BOOL GetJsonNodeEx(const Json::Value& root, LPCSTR lpsXPath, Json::Value& node);

/*
��õ�ǰ�������Json��
�ڲ�����c++�쳣
*/
BOOL GetJsonNodeStringUTF8(const Json::Value& root, CStringA& strValue);
BOOL GetJsonNodeStringUTF16(const Json::Value& root, CString& strValue);

/*
��õ�ǰ���Ķ��������ӽ���Json��
֧�ֶ༶�ӽ��
�ڲ�����c++�쳣
*/
BOOL GetJsonNodeStringUTF8(const Json::Value& root, LPCSTR lpsKey, CStringA& strValue);
BOOL GetJsonNodeStringUTF16(const Json::Value& root, LPCSTR lpsKey, CString& strValue);

/*
���ý��ֵ
֧�ֽ�����ת��Ϊ�ַ�����֧��\uXXXX����
�ڲ�����c++�쳣
*/
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LONG lValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, ULONG lValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LONGLONG llValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, ULONGLONG llValue, BOOL bAsString = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LPCSTR lpsValue, BOOL bEncode = FALSE);
BOOL SetJsonValue(Json::Value& node, LPCSTR lpsKey, LPCWSTR lpwsValue, BOOL bEncode = FALSE);


/*
��ʽ��Json��
�ڲ�����c++�쳣
*/

CUTFString RootToString(const Json::Value& root);
BOOL JsonWriteToStringUTF8( const Json::Value& root, CStringA& strJsonString );
BOOL JsonWriteToStringUTF16( const Json::Value& root, CString& strJsonString );