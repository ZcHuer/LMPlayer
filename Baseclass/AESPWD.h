#pragma once

#define AESPWD_KEY					"lxplxg"

#define AESPWD_MASK_1					0x79
#define AESPWD_MASK_2					0xAF
#define AESPWD_MASK_3					0x26
#define AESPWD_MASK_4					0xC5

#define MAKEBASEPWD(x, m) (BYTE)(((CHAR)#x[0])^(m))

#define AESPWD_MASK_AUTO		AESPWD_MASK_1
#define MAKEBASEPWDAUTO(x) MAKEBASEPWD(x, AESPWD_MASK_AUTO)
/*
BYTE g_byArrBasePWD[] = {
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(2),
	MAKEBASEPWDAUTO(3),
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(2),
	MAKEBASEPWDAUTO(3),
	/*
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(1),
	MAKEBASEPWDAUTO(1),
	* /
	AESPWD_MASK_AUTO,//×Ö·û´®µÄ½áÎ²·û
};
__inline LPBYTE AESPWD()
{
	static bool s_bInit = false;
	if(!s_bInit)
	{
		const DWORD dwCount = sizeof(g_byArrBasePWD);
		for(DWORD i = 0; i < dwCount; ++i)
			g_byArrBasePWD[i] = g_byArrBasePWD[i]^AESPWD_MASK_AUTO;
		s_bInit = true;
	}
	return g_byArrBasePWD;
}
*/
//#define AESPWD_KEY	AESPWD()