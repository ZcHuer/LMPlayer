#ifndef __UTIL_BUFFER_H__
#define __UTIL_BUFFER_H__

#include <vector>

typedef std::vector<char> Buffer;

inline Buffer& SetBuffer(Buffer &buf, size_t nLen) {
	buf.resize(nLen);
	return buf;
}

inline Buffer& SetBuffer(Buffer &buf, const void* data, size_t nLen) {
	buf.resize(nLen);
	memcpy(buf.data(), data, nLen);
	return buf;
}

inline Buffer& SetBuffer(Buffer &buf, const Buffer& from) {
	return SetBuffer(buf, from.data(), from.size());
}

inline Buffer& SetBuffer(Buffer &buf, const std::string & str) {
	return SetBuffer(buf, str.data(), str.size());
}


#endif//__UTIL_BUFFER_H__