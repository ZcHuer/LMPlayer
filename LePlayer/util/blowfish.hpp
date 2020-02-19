#ifndef __UTIL_BLOWFISH_HPP__
#define __UTIL_BLOWFISH_HPP__

#include "blowfish.h"
#include "buffer.h"
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

class CBlowFish {
public:

	CBlowFish() {}
	~CBlowFish() {}

public:

	void SetKey(const std::string& strKey) {
		BlowFishInit(&m_Blowfish, (unsigned char*)strKey.data(), strKey.length());
	}

	Buffer& GetOutput() {
		return m_Output;
	}

	char* GetOutputData() {
		return m_Output.data();
	}

	size_t GetOutputSize() {
		return m_Output.size();
	}

	std::string GetOutputHexString() {
		std::stringstream ss;
		for (size_t i = 0; i < m_Output.size(); i++)
			ss << std::hex << std::setw(2) << std::setfill('0') << (unsigned char)(m_Output[i]);

		return ss.str();
	}

	bool Encrypt(const std::string& str) {
		return Encrypt(str.data(), str.length());
	}

	bool Encrypt(const Buffer& buf) {
		return Encrypt(buf.data(), buf.size());
	}

	bool Encrypt(const void* buf, int len) {

		Buffer bufIn = padding(buf, len);

		m_Output.resize(bufIn.size());
		int nRet = BlowFishEncrypt(&m_Blowfish, (unsigned char*)bufIn.data(), bufIn.size(), (unsigned char*)m_Output.data(), m_Output.size(), ECB);
		if (nRet == 1)
			return true;
		else
			return false;

	}

	bool Decrypt(const std::string& str) {
		return Decrypt(str.data(), str.length());
	}

	bool Decrypt(const Buffer& buf) {
		return Decrypt(buf.data(), buf.size());
	}

	bool Decrypt(const void* buf, int len) {
		m_Output.resize(len);
		int nRet = BlowFishDecrypt(&m_Blowfish, (unsigned char*)buf, len, (unsigned char*)m_Output.data(), m_Output.size(), ECB);
		if (nRet == 1)
			return true;
		else
			return false;
	}

private:

	Buffer padding(const void* data, int len) {

		Buffer buf;

		int nNewLen = len;
		if (nNewLen % 8 != 0) {
			nNewLen = len + 8 - nNewLen % 8;
		}

		buf.resize(nNewLen);
		memset(buf.data(), 0, buf.size());
		memcpy(buf.data(), data, len);
		return buf;
	}

private:

	Buffer	 m_Output;
	Blowfish m_Blowfish;
	
};

#endif//__UTIL_BLOWFISH_HPP__