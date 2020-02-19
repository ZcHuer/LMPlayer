#ifndef GAMMARAMP_H_
#define GAMMARAMP_H_

#include "../include/Singleton.h"

/*
CGammaRamp class

Encapsulates the Gamma Ramp API and changes the brightness of 
the entire screen.

Written by Nir Sofer.
http://www.nirsoft.net


*/

class CGammaRamp : public pattern::CSingleton<CGammaRamp>
{
protected:
	HMODULE hGDI32;
	HDC hScreenDC;
	typedef BOOL (WINAPI *Type_SetDeviceGammaRamp)(HDC hDC, LPVOID lpRamp);

	Type_SetDeviceGammaRamp pGetDeviceGammaRamp;
	Type_SetDeviceGammaRamp pSetDeviceGammaRamp;

    int red_percent_;
    int green_percent_;
    int blue_percent_;

public:
    static BOOL IsDefaultGammaRamp(int red_percent, int green_percent, int blue_percent);

	CGammaRamp();
	~CGammaRamp();
    // 0 - 100, 0 表示不调整gamma
    BOOL SetSystemGammaRamp(int red_percent, int green_percent, int blue_percent);
    BOOL IsSupportGammaRamp();
    BOOL IsInDefault();
protected:
	BOOL LoadLibrary();
	void FreeLibrary();
	BOOL LoadLibraryIfNeeded();
	BOOL SetDeviceGammaRampImpl(HDC hDC, LPVOID lpRamp);
	BOOL GetDeviceGammaRampImpl(HDC hDC, LPVOID lpRamp);
	BOOL SetBrightness(HDC hDC, WORD wBrightness);
};



#endif