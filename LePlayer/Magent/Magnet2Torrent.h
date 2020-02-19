#pragma once
#include <string>
using namespace std;
class CMagnet2Torrent
{
public:
    CMagnet2Torrent();
    ~CMagnet2Torrent();

    void SetBasePath(const char* pPath);
    //string Magnet2Torrent(const char* pMagnet);
    string Magnet2Torrent2(const char* pMagnet);
private:
    int hexCharToInt(char c);
    void String2Byte(const char* pString,unsigned char* pByte,int nSize);

private:
    string m_strBasePath;
};

