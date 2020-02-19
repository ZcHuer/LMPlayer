#pragma once
#include "rijndael.h"

#define _AES_MAX_KEYLENGTH				32

#define _AES_SUPPORT_KEYLENGTH_16		16
#define _AES_SUPPORT_KEYLENGTH_24		24
#define _AES_SUPPORT_KEYLENGTH_32		32
#define _AES_SUPPORT_KEYLENGTH_COUNT	3

#define _AES_CRYPT_BITS_128				4
#define _AES_CRYPT_BITS_192				6
#define _AES_CRYPT_BITS_256				8

#define _AES_CRYPT_BITS					_AES_CRYPT_BITS_128
#define _AES_BLOCK_SIZE					(4*_AES_CRYPT_BITS)

#define _AES_FILE_READBUFFER_SIZE		(1024*1024*2*_AES_BLOCK_SIZE)		//£¨2*_AES_BLOCK_SIZE£©MB
#define _AES_FILE_CRYPTBUFFER_SIZE		(_AES_FILE_READBUFFER_SIZE + 512)

class CAESDecoder // only for ecb/aes128
{
public:
	CAESDecoder(void);
	~CAESDecoder(void);

	static LPBYTE _MAlloc(DWORD dwSize);
	static VOID   _MFree(LPBYTE lpbyBuf);

public:
	BOOL AESDecode(LPBYTE lpbyInput, DWORD dwInputLength, LPCSTR lpsKey, LPBYTE* lppOutput, DWORD* lpdwOutputLength);
	BOOL AESEncode(LPBYTE lpbyInput, DWORD dwInputLength, LPCSTR lpsKey, LPBYTE* lppOutput, DWORD* lpdwOutputLength);

	BOOL AESDecodeFile(HANDLE hFileSrc, HANDLE hFileDec, LPCSTR lpsKey);
	BOOL AESEncodeFile(HANDLE hFileSrc, HANDLE hFileEnc, LPCSTR lpsKey);

private:
	DWORD GetMaxKeyLength();
	DWORD GetSupportKeyLength(DWORD* lpdwSupportLengthArray, DWORD dwArraySize);

	DWORD FixKey(LPCSTR lpsKey, LPBYTE* lppFixedKey, DWORD* lpdwFixedKeyLength);

	BOOL IsBlockMode();
	DWORD GetBlockSize();

	BOOL Decode_ECB(LPBYTE lpbyInOut, DWORD& dwInOutLength);
	BOOL Encode_ECB(LPBYTE lpbyInOut, DWORD& dwInOutLength);

	BOOL InitDecInstance(LPBYTE lpbyKey, DWORD dwKeyLength);
	
private:
	RI m_riDecInstance;
	BOOL m_bAllocBuf;
};
