#pragma once
#include<string>
using namespace std;
class CnvEncoder
{
public:
	enum enCharset { none, utf8, gb2312, gbk };

	CnvEncoder();
	CnvEncoder(int nCharset);

	void SetCharset(int nCharset);

	static wstring ConvertUTF82Unicode(const string& sU8);

	const wchar_t* Ansi2Unicode(const char* szAnsi);
	const wchar_t* UTF82Unicode(const char* szU8, int len = -1);
	const char* UTF82UTF8(const char* szU8, int& cbSize);
	const char* GB2UTF8(const char* szGB, int& cbSize);
	string Unicode2Utf8(LPCWSTR wszString);
	string Unicode2GB2312(LPCWSTR wszUnicodeString);
	string URLEncode(const string& sSrc);
	
	void Release();

public:
	const char* ToUtf8(const char* szSrc, int& cbSize)
		{ return pfnEncode ? (this->*pfnEncode)(szSrc, cbSize) : NULL; }

private:
	void __wcsalloc(int nBytes);
	void __mballoc(int nBytes);

	const char* (CnvEncoder::*pfnEncode)(const char*, int& cbSize);
	wchar_t* wcsBuf;
	char* mbBuf;
	int wcsSize, mbSize;
};