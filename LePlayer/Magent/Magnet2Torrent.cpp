#include "Magnet2Torrent.h"
#include "base32.h"
#include <UrlMon.h>
#pragma comment(lib, "urlmon.lib")
using namespace Base32;

#include <wininet.h>
#pragma comment(lib, "wininet.lib")

CMagnet2Torrent::CMagnet2Torrent()
{
    m_strBasePath = "";
}

CMagnet2Torrent::~CMagnet2Torrent()
{
}

void CMagnet2Torrent::SetBasePath(const char * pPath)
{
    m_strBasePath = pPath;
    if (m_strBasePath[m_strBasePath.length()-1]!='\\')
    {
        m_strBasePath += "\\";
    }
}

/*
string CMagnet2Torrent::Magnet2Torrent(const char * pMagnet)
{
    string sTorrent = "";
    string sMagnet = pMagnet;
    int nPos = sMagnet.find("xt=");
    if (nPos == string::npos)
        return sTorrent;
    sMagnet = sMagnet.substr(nPos);
    nPos = sMagnet.find("urn:btih:");
    if (nPos == string::npos)
        return sTorrent;
    nPos += 9;
    sMagnet = sMagnet.substr(nPos);
    nPos = sMagnet.find("&");
    if (nPos!=string::npos)
    {
        sMagnet = sMagnet.substr(0,nPos);
    }
    unsigned char cBuff[20];
    String2Byte(sMagnet.c_str(), cBuff,20);
    string sBase32 = Base32::encode(cBuff,sizeof(cBuff));
    if (sBase32.empty())
    {
        return sTorrent;
    }
    
    string sUrl = "http://magnet.vuze.com/magnetLookup?hash="+ sBase32;

    sTorrent = m_strBasePath;
    sTorrent += sBase32 + ".torrent";
    while (URLDownloadToFileA(0, sUrl.c_str(), sTorrent.c_str(), 0, NULL)!=S_OK)
    {
        static int nCount = 0;
        if (nCount++>3)
        {
            sTorrent = "";
            break;
        }
    }
    return sTorrent;
}
*/

string CMagnet2Torrent::Magnet2Torrent2(const char * pMagnet)
{
    string sTorrent = "";
    string sMagnet = pMagnet;
    int nPos = sMagnet.find("xt=");
    if (nPos == string::npos)
        return sTorrent;
    sMagnet = sMagnet.substr(nPos);
    nPos = sMagnet.find("urn:btih:");
    if (nPos == string::npos)
        return sTorrent;
    nPos += 9;
    sMagnet = sMagnet.substr(nPos);
    nPos = sMagnet.find("&");
    if (nPos != string::npos){
        sMagnet = sMagnet.substr(0, nPos);
    }
    nPos = sMagnet.find("?");
    if (nPos != string::npos) {
        sMagnet = sMagnet.substr(0, nPos);
    }
    sMagnet = strupr((char*)sMagnet.c_str());
    string sUrl = "http://itorrents.org/torrent/" + sMagnet;
    sUrl.append(".torrent");
    char cBuff[MAX_PATH] = { 0 };
    GetTempPathA(MAX_PATH, cBuff);
    sTorrent = cBuff;
    sTorrent += sMagnet + ".torrent";
    DeleteUrlCacheEntryA(sUrl.c_str());
    while (URLDownloadToFileA(0, sUrl.c_str(), sTorrent.c_str(), 0, NULL) != S_OK)
    {
        static int nCount = 0;
        if (nCount++>3)
        {
            sTorrent = "";
            break;
        }
    }
    return sTorrent;
}

int CMagnet2Torrent::hexCharToInt(char c)
{
    if (c >= '0' && c <= '9') return (c - '0');
    if (c >= 'A' && c <= 'F') return (c - 'A' + 10);
    if (c >= 'a' && c <= 'f') return (c - 'a' + 10);
    return 0;
}

void CMagnet2Torrent::String2Byte(const char * pString, unsigned char * pByte, int nSize)
{
    if (strlen(pString)/2>nSize)
    {
        return;
    }

    string sSting = pString;
    unsigned int nByte;
    for (size_t i = 0; i < nSize; i++)
    {
        nByte = hexCharToInt(sSting[i * 2 + 0]);
        nByte = nByte * 16 + hexCharToInt(sSting[i * 2 + 1]);
        pByte[i] = nByte;
    }
}
