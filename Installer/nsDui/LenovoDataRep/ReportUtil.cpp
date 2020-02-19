#include "StdAfx.h"
#include "ReportUtil.h"
#include <corecrt_io.h>

extern CAes_encryptor g_aesEncry;
extern CString g_updateServiec;
extern CString g_updatePath;
extern CString g_version;
extern CString g_channelID;
extern CString g_updatePathPage;

extern CString g_InstallLePlayerserver ;
extern CString g_InstallLePlayerPath;

void SendRePort(CString id, WCHAR * detail)
{
	Json::Value jvData;
	//获取公共上报数据
	InitPublic(jvData);
	//return;
	string eventDeital = LeTools::ws2s(detail);
	jvData["actionTime"] = LeTools::GetTimeStamp();
	jvData["eventId"] = LeTools::ws2s(id.GetBuffer(0));
	jvData["eventType"] ="1";
	jvData["eventDes"] = eventDeital;

	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = LeTools::Gbk2Utf8(sData);

	string sEncrypt;
	sEncrypt = g_aesEncry.encrypt(sData);

	DWORD dwSize = sEncrypt.length() * 2;
	unsigned char* cDes = (unsigned char*)malloc(sEncrypt.length() * 2);
	memset(cDes, 0, dwSize);
	base64_encode((unsigned char*)sEncrypt.c_str(), sEncrypt.length(), cDes, &dwSize);
	sData = (char*)cDes;
	free(cDes);

	string postData = "encrypt=";
	postData.append(sData);
	postData.append("&rootname=c");
	
	string sResult = LeTools::doPostUrl(g_updateServiec.GetBuffer(0),
		g_updatePath.GetBuffer(0), postData);
	sResult = LeTools::ConvertUtf8ToGBK(sResult);
}

void SendRePortLePlayer(CString id, WCHAR * detail)
{
	Json::Value jvData;
	//获取公共上报数据
	InitPublic(jvData);
	//return;
	string eventDeital = LeTools::ws2s(detail);
	jvData["actionTime"] = LeTools::GetTimeStamp();
	jvData["eventId"] = LeTools::ws2s(id.GetBuffer(0));
	jvData["eventType"] = "1";
	jvData["eventDes"] = eventDeital;


	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = LeTools::replace(sData, "\n", "");
	sData = LeTools::Gbk2Utf8(sData);
	
	
	string sEncrypt = g_aesEncry.encrypt(sData);
	DWORD dwSize = sEncrypt.length() * 2;
	unsigned char* cDes = (unsigned char*)malloc(sEncrypt.length() * 2);
	memset(cDes, 0, dwSize);
	base64_encode((unsigned char*)sEncrypt.c_str(), sEncrypt.length(), cDes, &dwSize);
	sData = (char*)cDes;
	free(cDes);

	Json::Value jsPost;
	jsPost["encrypt"] = sData;
	sData = writer.write(jsPost);

	string sResult = LeTools::doPost(g_InstallLePlayerserver.GetBuffer(0),
		g_InstallLePlayerPath.GetBuffer(0), sData);
	sResult = LeTools::ConvertUtf8ToGBK(sResult);
}

void SendRePortPageEvent(CString pageID)
{

	Json::Value jvData;
	//获取公共上报数据
	InitPublic(jvData);
	//return;
	jvData["actionTime"] = LeTools::GetTimeStamp();
	jvData["eventId"] = "";
	jvData["eventType"] = "2";
	jvData["eventDes"] ="";

	
	Json::FastWriter writer;
	string sData = writer.write(jvData);
	sData = LeTools::Gbk2Utf8(sData);

	string sEncrypt;
	sEncrypt = g_aesEncry.encrypt(sData);

	DWORD dwSize = sEncrypt.length() * 2;
	unsigned char* cDes = (unsigned char*)malloc(sEncrypt.length() * 2);
	memset(cDes, 0, dwSize);
	base64_encode((unsigned char*)sEncrypt.c_str(), sEncrypt.length(), cDes, &dwSize);
	sData = (char*)cDes;
	free(cDes);

	string postData = "encrypt=";
	postData.append(sData);
	postData.append("&rootname=c");

	string sResult = LeTools::doPostUrl(g_updateServiec.GetBuffer(0),
		g_updatePathPage.GetBuffer(0), postData);
	sResult = LeTools::ConvertUtf8ToGBK(sResult);

}
void InitPublic(Json::Value &jvData)
{
	//添加公共数据
	/*
	"actionTime":"1533267296845",
	"lenovoId":"10122078515",
	"deviceId":"F832-D5CD-9449-E781-12D4-1868-3039-1D97",
	"appId":"1",
	"appVersion":"1.1.0",
	"cpId":"00",
	"appChannel":"1",
	"osType":"Windows10-10.0",
	"deviceStyle":"LENOVO ZHENGJIUZHE REN9000-25ICZ",
	"source":"1",
	*/
	
	CWmiInfo wmi;
	wmi.InitWmi();
	CString sResu;
	bool m_bIsWifi = wmi.IsWifi();
	//机型 精确到系列 比如 联想
	CString sItems[] = { L"Name", L"Vendor" };
	wmi.GetGroupItemInfo(L"Win32_ComputerSystemProduct", sItems, 2, sResu);
	string sDeviceStyle = LeTools::ws2s(sResu.GetBuffer(0));

	string m_sDeviceStyle = "";
	while (sDeviceStyle.find("\t") != -1)
	{
		m_sDeviceStyle.append(sDeviceStyle.substr(0, sDeviceStyle.find("\t")));
		m_sDeviceStyle.append(" ");
		sDeviceStyle = sDeviceStyle.substr(sDeviceStyle.find("\t") + 1);
	}

	m_sDeviceStyle = LeTools::trim(m_sDeviceStyle);
	m_sDeviceStyle.append(" ");
	m_sDeviceStyle.append(sDeviceStyle);

	sDeviceStyle = m_sDeviceStyle;
	m_sDeviceStyle = "";
	while (sDeviceStyle.find("\r\n") != -1)
	{
		m_sDeviceStyle.append(sDeviceStyle.substr(0, sDeviceStyle.find("\r\n")));
		sDeviceStyle = sDeviceStyle.substr(sDeviceStyle.find("\r\n") + 2);
	}
	//LeTools::replace(sDeviceStyle, "\r\n", "");
	m_sDeviceStyle.append(sDeviceStyle);

	//获取操作系统版本
	wmi.GetSingleItemInfo(L"Win32_OperatingSystem", L"Caption", sResu);
	string m_sOsType = LeTools::ws2s(sResu.GetBuffer(0));


	jvData["lenovoId"] = "";
	jvData["ifVip"] = "1";
	jvData["deviceId"] = LeTools::GetMacString();
	jvData["appId"] = "2";
	jvData["appVersion"] = LeTools::ws2s(g_version.GetBuffer());
	jvData["appChannel"] = "1";
	jvData["channel"] = LeTools::ws2s(g_channelID.GetBuffer()); 
	jvData["osType"] = m_sOsType;
	jvData["deviceStyle"] = m_sDeviceStyle;
	jvData["mac"] = LeTools::GetMacString();
	jvData["source"] ="lenovo";


	//MessageBoxA(NULL, GetDeviceId().c_str(), "", 0);
}

string CreateDevID(string file)
{
	if (_access(file.c_str(),0) ==-1)
	{
		WCHAR wszTraceConfig[MAX_PATH] = { 0 };
		SHGetSpecialFolderPath(NULL, wszTraceConfig, CSIDL_APPDATA, FALSE);
		PathAppend(wszTraceConfig, L"LenovoGameCenter\\config");
		CreateDirectory(wszTraceConfig, NULL);
	}
	string sRes = "";
	string sMac = LeTools::GetMacString();
	string sEncrypt="";
	sEncrypt = g_aesEncry.encrypt(sMac);
	MD5 md(sEncrypt.c_str());
	sEncrypt = md.toString();
	int len = 16;
	char tmp[64] = { 0 };
	strcpy(tmp, sEncrypt.c_str());
	string tmpHex = GetHexString(tmp,len);
	sRes = tmpHex.c_str();
	FILE * fp = fopen(file.c_str(), "w");
	if (fp)
	{
		fwrite(sRes.c_str(), 1, sRes.length(), fp);
		fclose(fp);
	}
	return sRes;
}
//获取DevID
string GetDeviceId()
{
	//return "F832-D5CD-9449-E781-12D4-1868-3039-1D97";
	WCHAR wszTraceConfig[MAX_PATH] = { 0 };
	string sRes="";
	SHGetSpecialFolderPath(NULL, wszTraceConfig, CSIDL_APPDATA, FALSE);
	PathAppend(wszTraceConfig, L"LenovoGameCenter\\config\\config.txt");
	string filePath = LeTools::ws2s(wszTraceConfig);
	if (_access(filePath.c_str(),0) !=-1)
	{
		//文件存在 
		FILE * fp = fopen(filePath.c_str(), "r");
		if (fp)
		{
			char buf[256] = { 0 };
			fread(buf, 1, 256, fp);
			sRes = buf;
			int len =sRes.find("\n");
			sRes = sRes.substr(0, len - 1);
			fclose(fp);
		}
		if (sRes.length()==0)
		{
			sRes =CreateDevID(filePath);
		}

	}
	else
	{
		//文件不存在
		sRes = CreateDevID(filePath);
	}
	return sRes;

}


string GetHexString(char* bt,int len)
{
	string s = "";
	for (int i = 0; i < len; i++)
	{
		char b = bt[i];
		int n, n1, n2;
		n = (int)b;
		n1 = n & 15;
		n2 = (n >> 4) & 15;
		if (n2 > 9)
			s += to_string(n2 - 10 + (int)'A');
		else
			s += to_string(n2);
		if (n1 > 9)
			s += to_string(n1 - 10 + (int)'A');
		else
			s += to_string(n1);
		if ((i + 1) != len && (i + 1) %2 == 0) s += "-";
	}
	return s;
}
string GetOSType()
{
	//return "Windows10 - 10.0";
	CWmiInfo wmi;
	wmi.InitWmi();
	CString sResu = _T("");
	wmi.GetSingleItemInfo(L"Win32_OperatingSystem", L"Caption", sResu);
	string sRes = LeTools::ws2s(sResu.GetBuffer(0));
	return sRes;

}


string trim(string &s)
{
	if (s.empty())
	{
		return s;
	}
	s.erase(0, s.find_first_not_of(" "));
	s.erase(s.find_last_not_of(" ") + 1);
	return s;
}
string GetDevStyle()
{
	//return "LENOVO ZHENGJIUZHE REN9000-25ICZ";
	CWmiInfo wmi;
	wmi.InitWmi();
	CString sResu = _T("");
	//Manufacturer Win32_ComputerSystem
	CString sItems[] = { L"Manufacturer" };
	wmi.GetGroupItemInfo(L"Win32_ComputerSystem", sItems, 1, sResu);
	string sDeviceStyle = LeTools::ws2s(sResu.GetBuffer(0));
	int index =sDeviceStyle.find('\t');
	sDeviceStyle = sDeviceStyle.substr(0, index);
	if (sDeviceStyle.length() == 0)
	{
		sDeviceStyle = "Unknown";
	}
	sResu = _T("");
	//Version Win32_ComputerSystemProduct
	CString sVersionItem[] = { "Version" };
	wmi.GetGroupItemInfo(L"Win32_ComputerSystemProduct", sVersionItem, 1, sResu);
	string sVersion = LeTools::ws2s(sResu.GetBuffer(0));

	index = sVersion.find('\t');
	sVersion = sVersion.substr(0, index);
	if (sVersion.length() == 0)
	{
		sVersion = "Unknown";
	}
	string m_sDeviceStyle = sDeviceStyle +" "+ sVersion;
// 	while (sDeviceStyle.find("\t") != -1)
// 	{
// 		m_sDeviceStyle.append(sDeviceStyle.substr(0, sDeviceStyle.find("\t")));
// 		m_sDeviceStyle.append(" ");
// 		sDeviceStyle = sDeviceStyle.substr(sDeviceStyle.find("\t") + 1);
// 	}

	m_sDeviceStyle = trim(m_sDeviceStyle);
	sDeviceStyle = m_sDeviceStyle;
	return sDeviceStyle;
}


#define SHA256_ROTL(a,b) (((a>>(32-b))&(0x7fffffff>>(31-b)))|(a<<b))
#define SHA256_SR(a,b) ((a>>b)&(0x7fffffff>>(b-1)))
#define SHA256_Ch(x,y,z) ((x&y)^((~x)&z))
#define SHA256_Maj(x,y,z) ((x&y)^(x&z)^(y&z))
#define SHA256_E0(x) (SHA256_ROTL(x,30)^SHA256_ROTL(x,19)^SHA256_ROTL(x,10))
#define SHA256_E1(x) (SHA256_ROTL(x,26)^SHA256_ROTL(x,21)^SHA256_ROTL(x,7))
#define SHA256_O0(x) (SHA256_ROTL(x,25)^SHA256_ROTL(x,14)^SHA256_SR(x,3))
#define SHA256_O1(x) (SHA256_ROTL(x,15)^SHA256_ROTL(x,13)^SHA256_SR(x,10))
char* StrSHA256(const char* str, long long length, char* sha256) {
	char *pp, *ppend;
	long l, i, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
	H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
	H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
	long K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
	};
	l = length + ((length % 64 > 56) ? (128 - length % 64) : (64 - length % 64));
	if (!(pp = (char*)malloc((unsigned long)l))) return 0;
	for (i = 0; i < length; pp[i + 3 - 2 * (i % 4)] = str[i], i++);
	for (pp[i + 3 - 2 * (i % 4)] = 128, i++; i < l; pp[i + 3 - 2 * (i % 4)] = 0, i++);
	*((long*)(pp + l - 4)) = length << 3;
	*((long*)(pp + l - 8)) = length >> 29;
	for (ppend = pp + l; pp < ppend; pp += 64) {
		for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
		for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
		A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
		for (i = 0; i < 64; i++) {
			T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
			T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
			H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
		}
		H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
	}
	free(pp - l);
	sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
	return sha256;
}
char* FileSHA256(const char* file, char* sha256)
{

	FILE* fh;
	char* addlp, T[64];
	long addlsize, j, W[64], T1, T2, A, B, C, D, E, F, G, H, H0, H1, H2, H3, H4, H5, H6, H7;
	long long length, i, cpys;
	void *pp, *ppend;
	H0 = 0x6a09e667, H1 = 0xbb67ae85, H2 = 0x3c6ef372, H3 = 0xa54ff53a;
	H4 = 0x510e527f, H5 = 0x9b05688c, H6 = 0x1f83d9ab, H7 = 0x5be0cd19;
	long K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
	};
	fh = fopen(file, "rb");
	fseek(fh, 0, SEEK_END);
	length = _ftelli64(fh);
	addlsize = (56 - length % 64 > 0) ? (64) : (128);
	if (!(addlp = (char*)malloc(addlsize))) return 0;
	cpys = ((length - (56 - length % 64)) > 0) ? (length - length % 64) : (0);
	j = (long)(length - cpys);
	if (!(pp = (char*)malloc(j))) return 0;
	fseek(fh, -j, SEEK_END);
	fread(pp, 1, j, fh);
	for (i = 0; i < j; addlp[i + 3 - 2 * (i % 4)] = ((char*)pp)[i], i++);
	free(pp);
	for (addlp[i + 3 - 2 * (i % 4)] = 128, i++; i < addlsize; addlp[i + 3 - 2 * (i % 4)] = 0, i++);
	*((long*)(addlp + addlsize - 4)) = length << 3;
	*((long*)(addlp + addlsize - 8)) = length >> 29;
	for (rewind(fh); 64 == fread(W, 1, 64, fh);) {
		for (i = 0; i < 64; T[i + 3 - 2 * (i % 4)] = ((char*)W)[i], i++);
		for (i = 0; i < 16; W[i] = ((long*)T)[i], i++);
		for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
		A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
		for (i = 0; i < 64; i++) {
			T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
			T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
			H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
		}
		H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
	}
	for (pp = addlp, ppend = addlp + addlsize; pp < ppend; pp = (long*)pp + 16) {
		for (i = 0; i < 16; W[i] = ((long*)pp)[i], i++);
		for (i = 16; i < 64; W[i] = (SHA256_O1(W[i - 2]) + W[i - 7] + SHA256_O0(W[i - 15]) + W[i - 16]), i++);
		A = H0, B = H1, C = H2, D = H3, E = H4, F = H5, G = H6, H = H7;
		for (i = 0; i < 64; i++) {
			T1 = H + SHA256_E1(E) + SHA256_Ch(E, F, G) + K[i] + W[i];
			T2 = SHA256_E0(A) + SHA256_Maj(A, B, C);
			H = G, G = F, F = E, E = D + T1, D = C, C = B, B = A, A = T1 + T2;
		}
		H0 += A, H1 += B, H2 += C, H3 += D, H4 += E, H5 += F, H6 += G, H7 += H;
	}
	free(addlp); fclose(fh);
	sprintf(sha256, "%08X%08X%08X%08X%08X%08X%08X%08X", H0, H1, H2, H3, H4, H5, H6, H7);
	return sha256;
}