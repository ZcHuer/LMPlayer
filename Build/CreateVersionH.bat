
@echo off
set svnrootpath=%~1
set curpath=%~dp0
cd %curpath%
echo svnrootpath=%svnrootpath%
echo curpath=%curpath%

::ȥ�ո�
SETLOCAL ENABLEDELAYEDEXPANSION

::������ð汾��
set prdver=""
set majorver=""
set minorver=""
set buildcount=0
set lastversion=0
for /F "tokens=1-2 delims==" %%i in (MainVer.ini) do ( 
	if /I "%%i"=="PRODUCTVERSION" (
		set prdver=%%j
  	) else 	if /I "%%i"=="MAJORVERSION" (
  		set majorver=%%j
  	) else 	if /I "%%i"=="MINORVERSION" (
  		set minorver=%%j
  	) else 	if /I "%%i"=="BUILD" (
  		set /a buildcount=%%j
  	) else if /I "%%i"=="LASTVER" (
  		set /a lastversion=%%j + 1	
  	)
)

echo ����version.h
del "%curpath%version.h"
>>"%curpath%version.h" echo.#define VERSION_STRING_PRODUCT				"%prdver%"
>>"%curpath%version.h" echo.#define VERSION_STRING_BUILD				"%majorver%.%minorver%.%buildcount%.%lastversion%"
>>"%curpath%version.h" echo.#define VERSION_STRING_BUILD_BATCMDUSE				 %majorver%.%minorver%.%buildcount%.%lastversion%
>>"%curpath%version.h" echo.#define VERSION_WSTRING_PRODUCT				_T(VERSION_STRING_PRODUCT)
>>"%curpath%version.h" echo.#define VERSION_WSTRING_BUILD 				_T(VERSION_STRING_BUILD)
>>"%curpath%version.h" echo.#define VERSION_SYS_FILE					%majorver%,%minorver%,%buildcount%,%lastversion%
>>"%curpath%version.h" echo.#define VERSION_SYS_PRODUCT					%majorver%,%minorver%,%buildcount%,%lastversion%
>>"%curpath%version.h" echo.#define VERSION_STRING_WIHTEBOARD			"1.1"
>>"%curpath%version.h" echo.#define VERSION_WSTRING_WIHTEBOARD			_T(VERSION_STRING_WIHTEBOARD)
>>"%curpath%version.h" echo.#define OBS_INTERFACEVERSION				_T("1")
>>"%curpath%version.h" echo.
>>"%curpath%version.h" echo.#define COMPANYNAME 						"Beijing Dashengzhixing Technology"
>>"%curpath%version.h" echo.#define COPYRIGHT 							"Copyright (c) Beijing Dashengzhixing Technology Co., Ltd."
>>"%curpath%version.h" echo.#define DISPLAY_FILE_VERSION  				VERSION_STRING_BUILD
>>"%curpath%version.h" echo.#define DISPLAY_PRODUCT_VERSION 			VERSION_STRING_BUILD
>>"%curpath%version.h" echo.#define SYS_FILE_VERSION  					VERSION_SYS_FILE
>>"%curpath%version.h" echo.#define SYS_PRODUCT_VERSION  				VERSION_SYS_PRODUCT
>>"%curpath%version.h" echo.#define PRODUCTNAME 						"ACTalk 2.0"

::�����µ�Mainver.ini
del /F/S/Q "%curpath%MainVer.ini.bak"
ren "%curpath%MainVer.ini" "MainVer.ini.bak"
>>"%curpath%MainVer.ini" echo.PRODUCTVERSION=%prdver%
>>"%curpath%MainVer.ini" echo.MAJORVERSION=%majorver%
>>"%curpath%MainVer.ini" echo.MINORVERSION=%minorver%
>>"%curpath%MainVer.ini" echo.BUILD=%buildcount%
>>"%curpath%MainVer.ini" echo.LASTVER=%lastversion%

exit
