/********************************************************************
//Created:		2010/05/06
//Filename: 	MD5.h
//Arranged:		Liu xiaochun 
//ToDo:			MD5 encode.
*********************************************************************/
#ifndef __MD5__H__
#define __MD5__H__
#include <string>
#include <cstdio>
#include <windows.h>  
/**
* @file CBMD5.h 
* @brief md5 algo
* @author chenxi
* @remark get the source from linxuan
* 大家都用这个吧，至少和python, php算的结果一样
*/
class CMD5Checksum
{
public:
	//interface functions for the RSA MD5 calculation	
	static std::string GetMD5(BYTE* pBuf, UINT nLength);

	//MD5 from file.
	static std::string GetMD5(FILE* input);

	//MD5 from file path segment
	static std::string GetMD5(const std::wstring& strPathFile, int nOffset, int nLength);

	//MD5 from file path.
	static std::string GetMD5(const std::string& strFilePath);
	
	static std::string GetMD5(LPCSTR lpstrFilePath);
	
	//MD5 From file wPath
	static std::string GetMD5(const std::wstring& wstrFilePath);
	
	static std::string GetMD5(LPCWSTR lpwstrFilePath);

protected:
	//constructor/destructor
	CMD5Checksum();

	virtual ~CMD5Checksum() {};

	//RSA MD5 implementation
	void Transform(BYTE Block[64]);	
	void Update(BYTE* Input, ULONG nInputLen);
	std::string Final();

	DWORD RotateLeft(DWORD x, int n);
	void FF( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	void GG( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	void HH( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);
	void II( DWORD& A, DWORD B, DWORD C, DWORD D, DWORD X, DWORD S, DWORD T);

	//utility functions
	void DWordToByte(BYTE* Output, DWORD* Input, UINT nLength);
	void ByteToDWord(DWORD* Output, BYTE* Input, UINT nLength);

private:
	BYTE  m_lpszBuffer[64];		//input buffer
	ULONG m_nCount[2];			//number of bits, modulo 2^64 (lsb first)
	ULONG m_lMD5[4];			//MD5 checksum
};

#endif //__MD5__H__