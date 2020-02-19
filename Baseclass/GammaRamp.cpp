#include "stdafx.h"
#include "gammaramp.h"

#include <windows.h>
#include <assert.h>
/*
CGammaRamp class

Encapsulates the Gamma Ramp API and changes the brightness of 
the entire screen.

Written by Nir Sofer.
http://www.nirsoft.net


*/


namespace {
    struct GammaTable
    {
        WORD redGamma[256];
        WORD greenGamma[256];
        WORD blueGamma[256];

        GammaTable()
        {
            ZeroMemory(redGamma, sizeof(redGamma));
            ZeroMemory(greenGamma, sizeof(greenGamma));
            ZeroMemory(blueGamma, sizeof(blueGamma));
        }
    };

    double szBuchang[101] = { 257.000000,
        257.000000, 257.000000, 255.713730, 254.427444, 253.141174,
        251.858826, 250.572556, 249.286270, 248.003922, 246.717651,
        245.431366, 244.149017, 242.862747, 241.576477, 240.294113,
        239.007843, 237.721573, 236.439209, 235.152939, 233.866669,
        232.584320, 231.298035, 230.011765, 228.729416, 227.443130,
        226.156860, 224.874512, 223.588242, 222.301956, 221.019608,
        219.733337, 218.447052, 217.164703, 215.878433, 214.592163,
        213.309799, 212.023529, 210.737259, 209.454895, 208.168625,
        206.882355, 205.600006, 204.313721, 203.027451, 201.741180,
        200.458817, 199.172546, 197.886276, 196.603928, 195.317642,
        194.031372, 192.749023, 191.462738, 190.176468, 188.894119,
        187.607849, 186.321564, 185.039215, 183.752945, 182.466660,
        181.184311, 179.898041, 178.611771, 177.329407, 176.043137,
        174.756866, 173.474503, 172.188232, 170.901962, 169.619614,
        168.333328, 167.047058, 165.764709, 164.478424, 163.192154,
        161.909805, 160.623535, 159.337250, 158.054901, 156.768631,
        155.482346, 154.199997, 152.913727, 151.627457, 150.341171,
        149.058823, 147.772552, 146.486267, 145.203918, 143.917648,
        142.631378, 141.349014, 140.062744, 138.776474, 137.494110,
        136.207840, 134.921570, 133.639221, 132.352936, 131.066666
    };

    void GeneralGammaArray(WORD szR[256], DWORD percent) {
        DWORD dwPercent = percent;
        if(dwPercent < 0)
            dwPercent = 0;
        else if(dwPercent > 100)
            dwPercent = 100;

        double diff = szBuchang[dwPercent];
        szR[0] = 0;
        for (int i = 1; i < 256; i++)
        {
            szR[i] = WORD(szR[i - 1] + diff);
        }
    }
}

CGammaRamp::CGammaRamp()
{
	//Initialize all variables.
	hGDI32 = NULL;
	hScreenDC = NULL;
	pGetDeviceGammaRamp = NULL;
	pSetDeviceGammaRamp = NULL;

    red_percent_ = 0;
    green_percent_ = 0;
    blue_percent_ = 0;
}

CGammaRamp::~CGammaRamp()
{
	FreeLibrary();
}


BOOL CGammaRamp::SetSystemGammaRamp(int red_percent, int green_percent, int blue_percent)
{
    static GammaTable tab;
    ZeroMemory(&tab, sizeof(GammaTable));

    GeneralGammaArray(tab.redGamma, red_percent);
    GeneralGammaArray(tab.greenGamma, green_percent);
    GeneralGammaArray(tab.blueGamma, blue_percent);

    CDC dc(::GetDC(NULL));
    BOOL success = SetDeviceGammaRampImpl(dc.m_hDC, &tab);
    if (success) {
        red_percent_ = red_percent;
        green_percent_ = green_percent;
        blue_percent_ = blue_percent;
    }

    assert(success);
    return success;
}

BOOL CGammaRamp::IsSupportGammaRamp()
{
    GammaTable tab;
    CDC dc(::GetDC(NULL));
    return GetDeviceGammaRampImpl(dc.m_hDC, &tab);
}

BOOL CGammaRamp::IsInDefault()
{
    return IsDefaultGammaRamp(red_percent_, green_percent_, blue_percent_);
}

BOOL CGammaRamp::IsDefaultGammaRamp(int red_percent, int green_percent, int blue_percent)
{
    return (0 == red_percent) && (0 == green_percent) && (0 == blue_percent);
}

BOOL CGammaRamp::LoadLibrary()
{
	BOOL bReturn = FALSE;

	FreeLibrary();
	//Load the GDI library.
	hGDI32 = ::LoadLibrary(L"gdi32.dll");
	if (hGDI32 != NULL)
	{
		//Get the addresses of GetDeviceGammaRamp and SetDeviceGammaRamp API functions.
		pGetDeviceGammaRamp = (Type_SetDeviceGammaRamp)GetProcAddress(hGDI32, "GetDeviceGammaRamp");
		pSetDeviceGammaRamp = (Type_SetDeviceGammaRamp)GetProcAddress(hGDI32, "SetDeviceGammaRamp");
		
		//Return TRUE only if these functions exist.
		if (pGetDeviceGammaRamp == NULL || pSetDeviceGammaRamp == NULL)
			FreeLibrary();
		else
			bReturn = TRUE;
	}

	return bReturn;
}


void CGammaRamp::FreeLibrary()
{
	//Free the GDI library.
	if (hGDI32 != NULL) 
	{
		::FreeLibrary(hGDI32);
		hGDI32 = NULL;
	}
}


BOOL CGammaRamp::LoadLibraryIfNeeded()
{
	BOOL bReturn = FALSE;

	if (hGDI32 == NULL)
		LoadLibrary();

	if (pGetDeviceGammaRamp != NULL && pSetDeviceGammaRamp != NULL)
		bReturn = TRUE;

	return bReturn;
}


BOOL CGammaRamp::SetDeviceGammaRampImpl(HDC hDC, LPVOID lpRamp)
{
	//Call to SetDeviceGammaRamp only if this function is successfully loaded.
	if (LoadLibraryIfNeeded())
	{
		return pSetDeviceGammaRamp(hDC, lpRamp);
	}
	else
		return FALSE;
}


BOOL CGammaRamp::GetDeviceGammaRampImpl(HDC hDC, LPVOID lpRamp)
{
	//Call to GetDeviceGammaRamp only if this function is successfully loaded.
	if (LoadLibraryIfNeeded())
	{
		return pGetDeviceGammaRamp(hDC, lpRamp);
	}
	else
		return FALSE;

}


BOOL CGammaRamp::SetBrightness(HDC hDC, WORD wBrightness)
{
	/*
	Changes the brightness of the entire screen.
	This function may not work properly in some video cards.

	The wBrightness value should be a number between 0 and 255.
	128 = Regular brightness
	above 128 = brighter
	below 128 = darker

    If hDC is NULL, SetBrightness automatically load and release 
	the display device context for you.

	*/
	BOOL bReturn = FALSE;
	HDC hGammaDC = hDC;

	//Load the display device context of the entire screen if hDC is NULL.
	if (hDC == NULL)
		hGammaDC = GetDC(NULL);

	if (hGammaDC != NULL)
	{
		//Generate the 256-colors array for the specified wBrightness value.
		WORD GammaArray[3][256];

		for (int iIndex = 0; iIndex < 256; iIndex++)
		{
			int iArrayValue = iIndex * (wBrightness + 128);

			if (iArrayValue > 65535)
				iArrayValue = 65535;

			GammaArray[0][iIndex] = 
			GammaArray[1][iIndex] = 
			GammaArray[2][iIndex] = (WORD)iArrayValue;
			
		}

		//Set the GammaArray values into the display device context.
		bReturn = SetDeviceGammaRampImpl(hGammaDC, GammaArray);
	}

	if (hDC == NULL)
		ReleaseDC(NULL, hGammaDC);

	return bReturn;
}