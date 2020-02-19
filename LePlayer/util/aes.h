//#ifndef __UTIL_AES_H__
//#define __UTIL_AES_H__
//
//
//#include "buffer.h"
//#include <vector>
//#include <string>
//#include <sstream>
//#include <iomanip>
//#include <openssl/aes.h>
//
//class CAes {
//public:
//
//	CAes() {}
//	~CAes() {}
//
//public:
//
//	bool SetEncryptKey(const std::string& strKey) {
//		if (AES_set_encrypt_key((const unsigned char*)strKey.c_str(), 256, &m_AESkey) < 0)
//			return false;
//
//		return true;
//	}
//
//	bool SetDecryptKey(const std::string& strKey) {
//		if (AES_set_decrypt_key((const unsigned char*)strKey.c_str(), 256, &m_AESkey) < 0)
//			return false;
//
//		return true;
//	}
//
//
//	Buffer& GetOutput() {
//		return m_Output;
//	}
//
//	char* GetOutputData() {
//		return m_Output.data();
//	}
//
//	size_t GetOutputLength() {
//		return m_Output.size();
//	}
//
//	std::string GetOutputHexString() {
//		std::stringstream ss;
//		for (size_t i = 0; i < m_Output.size(); i++)
//			ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)(m_Output[i]);
//
//		return ss.str();
//	}
//
//	bool Encrypt(const std::string& str,bool bPadding ) {
//		return Encrypt(str.data(), str.length(), bPadding);
//	}
//
//	bool Encrypt(const Buffer& buf, bool bPadding ) {
//		return Encrypt(buf.data(), buf.size(),bPadding);
//	}
//
//	bool Encrypt(const void* buf, int len, bool bPadding ) {
//
//		Buffer in;
//		if (bPadding)
//			SetBuffer(in, padding(buf, len));
//		else
//			SetBuffer(in, buf, len);
//		SetBuffer(m_Output, in);
//
//		int _len = in.size(), en_len = 0;
//		_len -= _len%AES_BLOCK_SIZE;
//		char* pIN = (char*)in.data();
//		char* pOUT = (char*)m_Output.data();
//		while (en_len < _len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制  
//		{
//			AES_encrypt((unsigned char*)pIN, (unsigned char*)pOUT, &m_AESkey);
//			pIN += AES_BLOCK_SIZE;
//			pOUT += AES_BLOCK_SIZE;
//			en_len += AES_BLOCK_SIZE;
//		}
//
//		if (en_len == 0)
//			return false;
//
//		return true;
//
//	}
//
//	bool Decrypt(const std::string& str) {
//		return Decrypt(str.data(), str.length());
//	}
//
//	bool Decrypt(const Buffer& buf) {
//		return Decrypt(buf.data(), buf.size());
//	}
//
//	bool Decrypt(const void* buf, int len) {
//
//		Buffer in;
//		SetBuffer(in, buf, len);
//		SetBuffer(m_Output, in);
//
//		int _len = in.size(), de_len = 0;
//		_len -= _len % AES_BLOCK_SIZE;
//		char* pIN = (char*)in.data();
//		char* pOUT = (char*)m_Output.data();
//		while (de_len < _len)//输入输出字符串够长，并且是AES_BLOCK_SIZE的整数倍，需要严格限制  
//		{
//			AES_decrypt((unsigned char*)pIN, (unsigned char*)pOUT, &m_AESkey);
//			pIN += AES_BLOCK_SIZE;
//			pOUT += AES_BLOCK_SIZE;
//			de_len += AES_BLOCK_SIZE;
//		}
//
//		if (de_len == 0)
//			return false;
//
//		return true;
//	}
//
//private:
//
//	Buffer padding(const void* data, int len) {
//
//		Buffer buf;
//
//		int nBlockSize = AES_BLOCK_SIZE;
//
//		int nNewLen = len;
//		if (nNewLen % nBlockSize != 0) {
//			nNewLen = len + nBlockSize - nNewLen % nBlockSize;
//		}
//
//		buf.resize(nNewLen);
//		memset(buf.data(), 0, buf.size());
//		memcpy(buf.data(), data, len);
//		return buf;
//	}
//
//private:
//
//	Buffer	 m_Output;
//	AES_KEY  m_AESkey;
//
//};
//
//#endif//__UTIL_AES_H__