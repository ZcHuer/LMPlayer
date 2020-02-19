#include "StdAfx.h"
#include "DefaultFileAssociate.h"


CDefaultFileAssociate::CDefaultFileAssociate(void)
{
}


CDefaultFileAssociate::~CDefaultFileAssociate(void)
{
}

typedef struct _tagDefExt
{
	LPCWSTR lpExt;
	LPCWSTR lpAssociate;
}DEFEXT,*PDEFEXT;

LPCWSTR CDefaultFileAssociate::GetDefaultFileAssociate(LPCWSTR lpExt)
{
	if( IsWindowVistaLatter() )
		return GetDefault_Win7(lpExt);

	return GetDefault_XP(lpExt);
}

DEFEXT _DefExts_XP[] = 
{
	{_T("asf"),_T("ASFFile")},
	{_T("wm"),_T("ASFFile")},
	{_T("wmv"),_T("WMVFile")},
	{_T("wma"),_T("WMAFile")},
	{_T("aif"),_T("AIFFFile")},
	{_T("aifc"),_T("AIFFFile")},
	{_T("aiff"),_T("AIFFFile")},
	{_T("avi"),_T("avifile")},
	{_T("ivf"),_T("IVFfile")},
	{_T("m1v"),_T("mpegfile")},
	{_T("mpe"),_T("mpegfile")},
	{_T("mpeg"),_T("mpegfile")},
	{_T("mpg"),_T("mpegfile")},
	{_T("au"),_T("AUFile")},
	{_T("cda"),_T("CDAFile")},
	{_T("mid"),_T("midfile")},
	{_T("midi"),_T("midfile")},
	{_T("mp2"),_T("mpegfile")},
	{_T("mp3"),_T("mp3file")},
	{_T("mpa"),_T("mpegfile")},
	{_T("rmi"),_T("midfile")},
	{_T("wav"),_T("soundrec")},
	{_T("asx"),_T("ASXFile")},
	{_T("m3u"),_T("m3ufile")},
	{_T("wax"),_T("WAXFile")},
	{_T("wmx"),_T("ASXFile")},
	{_T("wvx"),_T("WVXFile")},
};

LPCWSTR CDefaultFileAssociate::GetDefault_XP(LPCWSTR lpExt)
{
	for( int i=0; i<ARRAYSIZE(_DefExts_XP); i++ )
	{
		if( _wcsicmp(_DefExts_XP[i].lpExt, lpExt) == 0 )
		{
			return _DefExts_XP[i].lpAssociate;
		}
	}

	return NULL;
}

DEFEXT _DefExts_Vista[] = 
{
	{_T("asf"),_T("WMP11.AssocFile.ASF")},
	{_T("wm"),_T("WMP11.AssocFile.ASF")},
	{_T("wmv"),_T("WMP11.AssocFile.WMV")},
	{_T("wma"),_T("WMP11.AssocFile.WMA")},
	{_T("mov"),_T("WMP11.AssocFile.MOV")},
	{_T("aif"),_T("WMP11.AssocFile.AIFF")},
	{_T("aifc"),_T("WMP11.AssocFile.AIFF")},
	{_T("aiff"),_T("WMP11.AssocFile.AIFF")},
	{_T("3g2"),_T("WMP11.AssocFile.3G2")},
	{_T("3gp"),_T("WMP11.AssocFile.3GP")},
	{_T("3gp2"),_T("WMP11.AssocFile.3G2")},
	{_T("3gpp"),_T("WMP11.AssocFile.3GP")},
	{_T("avi"),_T("WMP11.AssocFile.AVI")},
	{_T("m1v"),_T("WMP11.AssocFile.MPEG")},
	{_T("m2ts"),_T("WMP11.AssocFile.M2TS")},
	{_T("m2v"),_T("WMP11.AssocFile.MPEG")},
	{_T("m4v"),_T("WMP11.AssocFile.MP4")},
	{_T("mp4"),_T("WMP11.AssocFile.MP4")},
	{_T("mpe"),_T("WMP11.AssocFile.MPEG")},
	{_T("mpeg"),_T("WMP11.AssocFile.MPEG")},
	{_T("mpg"),_T("WMP11.AssocFile.MPEG")},
	{_T("mts"),_T("WMP11.AssocFile.M2TS")},
	{_T("ts"),_T("WMP11.AssocFile.TTS")},
	{_T("aac"),_T("WMP11.AssocFile.ADTS")},
	{_T("au"),_T("WMP11.AssocFile.AU")},
	{_T("cda"),_T("WMP11.AssocFile.CDA")},
	{_T("m4a"),_T("WMP11.AssocFile.M4A")},
	{_T("mid"),_T("WMP11.AssocFile.MIDI")},
	{_T("midi"),_T("WMP11.AssocFile.MIDI")},
	{_T("mp2"),_T("WMP11.AssocFile.MP3")},
	{_T("mp3"),_T("WMP11.AssocFile.MP3")},
	{_T("mpa"),_T("WMP11.AssocFile.MPEG")},
	{_T("mod"),_T("WMP11.AssocFile.MPEG")},
	{_T("rmi"),_T("WMP11.AssocFile.MIDI")},
	{_T("wav"),_T("WMP11.AssocFile.WAV")},
	{_T("asx"),_T("WMP11.AssocFile.ASX")},
	{_T("m3u"),_T("WMP11.AssocFile.m3u")},
	{_T("wax"),_T("WMP11.AssocFile.WAX")},
	{_T("wmx"),_T("WMP11.AssocFile.ASX")},
	{_T("wvx"),_T("WMP11.AssocFile.WVX")},
};

LPCWSTR CDefaultFileAssociate::GetDefault_Win7(LPCWSTR lpExt)
{
	for( int i=0; i<ARRAYSIZE(_DefExts_Vista); i++ )
	{
		if( _wcsicmp(_DefExts_Vista[i].lpExt, lpExt) == 0 )
		{
			return _DefExts_Vista[i].lpAssociate;
		}
	}

	return NULL;
}
