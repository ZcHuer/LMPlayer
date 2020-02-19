#pragma once
#include <time.h>

#include "LeTools.h"
#include "json/json.h"

//define funcitons
string GetOSType();
string GetOSType();
string GetDevStyle();
string CreateDevID(string file);
void SendRePort(CString id, WCHAR * detail);
void SendRePortLePlayer(CString id, WCHAR * detail);
string GetDeviceId();
void InitPublic(Json::Value &jvData);
string GetHexString(char* bt,int len);
char* StrSHA256(const char* str, long long length, char* sha256);
char* FileSHA256(const char* file, char* sha256);

void SendRePortPageEvent(CString pageID);
