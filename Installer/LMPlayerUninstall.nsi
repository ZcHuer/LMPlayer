
# ======================LMPlayer uninstall program==============================
!define PRODUCT_NAME           "LMPlayer" 
!define EXE_NAME               "LMPlayer.exe" 
!define EXE_FILENAME 		   "LMPlayer.exe"
!define INST_SUBDIR            "" 
!define PRODUCT_VERSION        "2.0.2.17" 
!define PRODUCT_PUBLISHER      "LMPlayer" 
!define PRODUCT_LEGAL          "Copyright (C) 1998-2020 Lenovo, All Rights Reserved" 
!define CONFIGINI_NAME_PRE     "info"
!define LICENSETXT_FILE_PRE       "license" 
!define SKINFILEXML_PRE           "install" 
!define SKINFILEXML_UNINST_PRE           "uninstall" 
!define PCODE                  "" 
!define TIME_COMPILE_INSTALLER "2020-4-30"


!define WIDTH 600 
!define HEIGHT 340 
!define WIDTH_MORE 600 
!define HEIGHT_MORE 440 
!define INNER_POS "10,30,590,335" 
!define INNER_POS_MORE  "10,30,590,435" 

!define UINST_EXE_NAME "uninst.exe"
!define TEMP_USEDATA_DIR "$APPDATA\Lenovo\lmp" 
!define GAME_INSTALL_DIR ""
!define /date PRODUCT_DATE %Y%m%d

# ==================== NSIS?ǥ ================================
;SetCompressor /SOLID LZMA
;SetCompressor zlib

# ===================== ??????? =============================
!include "LogicLib.nsh"
!include "nsDialogs.nsh"
!include "include\common.nsh"
!include "StdUtils.nsh"

# ===================== Ǔ?ǞǸ? =============================
VIProductVersion                    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${EXE_NAME}"
VIAddVersionKey "FileDescription"   "${PRODUCT_NAME}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

# ===================== ???? ==============================
var vPROMPT_SELECT_INSTALL_DIR
var vDESKTOP_SHORTCUT_NAME
var vSHORTCUTSUBDIR
var vPROGRAM_SHUORTCUT_NAME
var vCTROLPANEL_UNINSTALL_NAME
var vun_exeisrunning

var vLanguageId
var vLangIDPath
var vGame_install_dir
var Var_IniFile
Var vServerUrl
Var vPruduceVersion
Var vChannelID
var vConifgPath


!define PRODUCT_UNIST_CONTRL_PATH "${PRODUCT_NAME}${PCODE}"
!define INSTALLNAME "LMPlayer${PCODE}${PRODUCT_DATE}.exe"

; Ǔ?Ǟ??.
Name "${PRODUCT_NAME}"

# Ǔ??Ǔ???.
OutFile "LMPlayer${PCODE}${PRODUCT_DATE}.exe"

InstallDir "D:\${PRODUCT_PUBLISHER}"

!macro serverCfg
	StrCpy $vServerUrl "https://LMPlayer-dev.vgs.lenovo.com.cn/"
	StrCpy $vPruduceVersion "${PRODUCT_VERSION}"
!macroend

!macro ReadVersionCfg
	SetShellVarContext current
	
	SetOutPath "$TEMP\\lmpskin"
    File /r Config.ini
	StrCpy $vConifgPath "$TEMP\\lmpskin\\Config.ini"
	;messagebox mb_ok "$INSTDIR\config.ini"
	ReadINIStr $vPruduceVersion $vConifgPath Update CurVer
	ReadINIStr $vChannelID $vConifgPath Channel ChannelID
	;messagebox mb_ok $vPruduceVersion
!macroend

!macro ReadVersionCfgUnistall
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	;messagebox mb_ok $INSTDIR
	
	ReadINIStr $vChannelID "$INSTDIR\config.ini" Channel ChannelID
	ReadINIStr $vPruduceVersion "$INSTDIR\config.ini" Update CurVer
	;messagebox mb_ok "$vPruduceVersion+$INSTDIR/config.ini" 
!macroend

RequestExecutionLevel admin

Icon              "image\logo.ico"
UninstallIcon     "image\unlogo.ico"



# ?????
Page custom DUIPage


# ǘ??Ǔ????
UninstPage custom un.DUIPage

# ======================= DUILIB ????? =========================
Var hInstallDlg

Var sReserveData   #ǘ?????t?? 
Var InstallState   #??Ǔ????Ǔ???  
Var UnInstallValue  #ǘ????  
Var vprocess
Var vSetAsDefaultBrowser ;??????ar vCreateDesktopShortcut ;????????
Var vAddStartShortCut ;???????
Var vAddQuickStart ;????Ȃ??

Var vCallOnce ;???????

Var vInitPath ;?????
Var vUserData
Var vBdelteGame
Var vServerIni
AllowSkipFiles off

Function .onInit
	InitPluginsDir
	System::Call 'kernel32::CreateMutexA(i 0, i 0, t "WinSnap_installer") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
    ;MessageBox MB_OK|MB_ICONEXCLAMATION "??? WinSnap Ǔ??????ǡȢ"
    Abort
FunctionEnd
Section .init
	IfSilent 0 +2
	push "true"
	;call installSlient 
	!insertmacro serverCfg
    ;MessageBox MB_OK|MB_ICONINFORMATION 'This is a "silent" installer'
	;messagebox mb_ok $INSTDIR
    # there is no need to use IfSilent for this one because the /SD switch takes care of that
    ;MessageBox MB_OK|MB_ICONINFORMATION "This is not a silent installer"
SectionEnd

Function GetCurrLangID
System::Call "Kernel32::GetUserDefaultUILanguage(v ..) i .s"
Pop $0
IntOp $0 $0 & 0xFFFF

${if} $0 == 2052
	StrCpy $vLanguageId 0804
${elseif} $0 == 1028
	StrCpy $vLanguageId 0404
${elseif} $0 == 3076
	StrCpy $vLanguageId 0404
${elseif} $0 == 4100
	StrCpy $vLanguageId 0404
${elseif} $0 == 5124
	StrCpy $vLanguageId 0404
;default use english
${else}
	StrCpy $vLanguageId 0409
${EndIf}
	System::Free $0
FunctionEnd

Function un.GetCurrLangID
System::Call "Kernel32::GetUserDefaultUILanguage(v ..) i .s"
Pop $0
IntOp $0 $0 & 0xFFFF

${if} $0 == 2052
	StrCpy $vLanguageId 0804
${elseif} $0 == 1028
	StrCpy $vLanguageId 0404
${elseif} $0 == 3076
	StrCpy $vLanguageId 0404
${elseif} $0 == 4100
	StrCpy $vLanguageId 0404
${elseif} $0 == 5124
	StrCpy $vLanguageId 0404
;default use english
;default use english
${else}
	StrCpy $vLanguageId 0409
${EndIf}
	System::Free $0
FunctionEnd


Section un.init

	call un.LangTransfer
	!insertmacro serverCfg
	IfSilent 0 +2
	push "true"
	call un.slientUninstall
	
SectionEnd 
Function DUIPage
	InitPluginsDir
	;C:\Users\Administrator\AppData\Roaming\LePlayerCenter
	call LangTransfer
	!insertmacro ReadVersionCfg
	
	StrCpy $vInitPath "$APPDATA"
	
	
    SetOutPath "$TEMP\\lmpskin"
	RMDir /r "$TEMP\\lmpskin"
    File /r skin\*.*
	
    nsDui::InitDUISetup "install" "$TEMP\lmpskin" "${SKINFILEXML_PRE}$vLanguageId.xml" "$vInitPath\${LICENSETXT_FILE_PRE}$vLanguageId.txt" "wizardTab" 0 "showPic" 1000
    Pop $hInstallDlg
	nsDui::SetDirValue "$vInitPath"
	;第一页
	;关闭
    nsDui::FindControl "btnClose"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnExitDUISetup
        nsDui::OnControlBindNSISScript "btnClose" $0
    ${EndIf}
	;最小化
	nsDui::FindControl "btnMin"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnBtnMin
        nsDui::OnControlBindNSISScript "btnMin" $0
    ${EndIf}
	;点击快速安装
	nsDui::FindControl "QuickInstall"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnQuickInstall
        nsDui::OnControlBindNSISScript "QuickInstall" $0
    ${EndIf}
	;选择目录
	nsDui::FindControl "DIYInstall"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnDIYInstall
        nsDui::OnControlBindNSISScript "DIYInstall" $0
    ${EndIf}
	
	;选择目录
	nsDui::FindControl "Agree"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnAgree
        nsDui::OnControlBindNSISScript "Agree" $0
    ${EndIf}
	
	;选择目录
	nsDui::FindControl "btnAgree"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnBtnAgree
        nsDui::OnControlBindNSISScript "btnAgree" $0
    ${EndIf}
	
	;第二页
	nsDui::FindControl "btnBack"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnbtnBack
        nsDui::OnControlBindNSISScript "btnBack" $0
    ${EndIf}
	;btnSelectDir
	
	nsDui::FindControl "btnSelectDir"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnBtnSelectDir
        nsDui::OnControlBindNSISScript "btnSelectDir" $0
    ${EndIf}
	
	nsDui::FindControl "btnInstall"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnBtnInstall
        nsDui::OnControlBindNSISScript "btnInstall" $0
    ${EndIf}
	
	
	nsDui::FindControl "btnfinish"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnFinished
        nsDui::OnControlBindNSISScript "btnfinish" $0
    ${EndIf}
	
    ; ?? xxx ?????ǡ??
    GetFunctionAddress $0 OnBtnInstall
    nsDui::ShowPage "" $0 2000
	
	;nsDui::SelectPage "wizardTab" 2
InstallAbort:
FunctionEnd
Function OnbtnBack
	nsDui::PrePage "wizardTab"
FunctionEnd
Function OnQuickInstall
	call OnBtnInstall
FunctionEnd

Function OnDIYInstall
	nsDui::NextPage "wizardTab"
FunctionEnd

!macro languagetrans
ReadINIStr $vPROMPT_SELECT_INSTALL_DIR $vLangIDPath Installation PROMPT_SELECT_INSTALL_DIR
ReadINIStr $vSHORTCUTSUBDIR $vLangIDPath Installation SHORTCUTSUBDIR
ReadINIStr $vDESKTOP_SHORTCUT_NAME $vLangIDPath Installation DESKTOP_SHORTCUT_NAME
ReadINIStr $vPROGRAM_SHUORTCUT_NAME $vLangIDPath Installation PROGRAM_SHUORTCUT_NAME
ReadINIStr $vCTROLPANEL_UNINSTALL_NAME $vLangIDPath Installation CTROLPANEL_UNINSTALL_NAME
ReadINIStr $vun_exeisrunning $vLangIDPath Installation un_exeisrunning
!macroend

Function un.DUIPage
	!insertmacro serverCfg
	!insertmacro ReadVersionCfgUnistall
    StrCpy $InstallState "0"
    InitPluginsDir
    call un.LangTransfer
    SetOutPath "$TEMP\\lmpskin"
    File /r skin\*.*
    nsDui::InitDUISetup "uninstall" "$TEMP\\lmpskin" "${SKINFILEXML_UNINST_PRE}$vLanguageId.xml" "" "$vInitPath\${LICENSETXT_FILE_PRE}$vLanguageId.txt" "wizardTab" 0 "" 1500
    Pop $hInstallDlg
    Call un.BindUnInstUIControls
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "60" "uninstal" "$vPruduceVersion" $vChannelID
    nsDui::ShowPage
FunctionEnd

Function un.BindUnInstUIControls
	;page1
	nsDui::FindControl "btnUnInstallRep"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnRepire
        nsDui::OnControlBindNSISScript "btnUnInstallRep" $0
    ${EndIf}
	
	nsDui::FindControl "btnPage11"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.GoToPage2
        nsDui::OnControlBindNSISScript "btnPage11" $0
    ${EndIf}
	
	nsDui::FindControl "btnUnInstall"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.onUninstall
        nsDui::OnControlBindNSISScript "btnUnInstall" $0
    ${EndIf}
	
	;btnGoPage1
	
	nsDui::FindControl "btnGoPage1"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.btnGoPage1
        nsDui::OnControlBindNSISScript "btnGoPage1" $0
    ${EndIf}
	
	;btnUnInstallGo
	nsDui::FindControl "btnUnInstallGo"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.btnUnInstallGo
        nsDui::OnControlBindNSISScript "btnUnInstallGo" $0
    ${EndIf}
	;btnbtnSave1
	
	nsDui::FindControl "btnbtnSave1"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.ExitDUISetup
        nsDui::OnControlBindNSISScript "btnbtnSave1" $0
    ${EndIf}
	;public function 
	nsDui::FindControl "btnUninstallCancel"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.ExitDUISetup
        nsDui::OnControlBindNSISScript "btnUninstallCancel" $0
    ${EndIf}
	
    nsDui::FindControl "btnUninstallClose"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.ExitDUISetup
        nsDui::OnControlBindNSISScript "btnUninstallClose" $0
    ${EndIf}
    
	nsDui::FindControl "btnUninstallMin"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnBtnMin
        nsDui::OnControlBindNSISScript "btnUninstallMin" $0
    ${EndIf}
	
    nsDui::FindControl "btnUninstallFinished"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.btnUninstallFinished
        nsDui::OnControlBindNSISScript "btnUninstallFinished" $0
    ${EndIf}
	
	
	nsDui::FindControl "btnClose"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnExitDUISetup
        nsDui::OnControlBindNSISScript "btnClose" $0
    ${EndIf}
	;最小化
	nsDui::FindControl "btnMin"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnBtnMin
        nsDui::OnControlBindNSISScript "btnMin" $0
    ${EndIf}
	
	;选择目录
	nsDui::FindControl "ShowPic0"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnShowPic0
        nsDui::OnControlBindNSISScript "ShowPic0" $0
    ${EndIf}
	
	nsDui::FindControl "ShowPic1"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnShowPic1
        nsDui::OnControlBindNSISScript "ShowPic1" $0
    ${EndIf}
	
	nsDui::FindControl "ShowPic2"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 un.OnShowPic2
        nsDui::OnControlBindNSISScript "ShowPic2" $0
    ${EndIf}
FunctionEnd

Function un.OnShowPic0
	;
	ReadRegStr $vInitPath HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $vInitPath == ""
		ReadRegStr $vInitPath HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	nsDui::LunchApp 0 "$vInitPath\${EXE_NAME}"
	nsDui::ExitDUISetup
	
FunctionEnd
Function un.OnShowPic1
	ReadRegStr $vInitPath HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $vInitPath == ""
		ReadRegStr $vInitPath HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	nsDui::LunchApp 1 "$vInitPath\${EXE_NAME}"
	nsDui::ExitDUISetup
FunctionEnd
Function un.OnShowPic2

	ReadRegStr $vInitPath HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $vInitPath == ""
		ReadRegStr $vInitPath HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	nsDui::LunchApp 2 "$vInitPath\${EXE_NAME}"
	nsDui::ExitDUISetup
FunctionEnd

Function un.btnUninstallFinished
;ExecShell open "https://sta.vgs.lenovo.com.cn/feedback.html"
ExecShell open "http://sta.vgs.lenovo.com.cn/lmplayer.html"
nsDui::ExitDUISetup
FunctionEnd
Function un.OnRepire
	;start repired exe
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	Exec '"$INSTDIR\run\LeRepair.exe" LeRepair'
	nsDui::ExitDUISetup
FunctionEnd

Function un.btnbtnSave1
		;ExitDUISetup
FunctionEnd 
Function un.btnUnInstallGo
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "41" "1uninstal" "$vPruduceVersion" $vChannelID
	nsDui::NextPage "wizardTab"
FunctionEnd
Function un.GoToPage2
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "41" "2uninstall" "$vPruduceVersion" $vChannelID
	nsDui::SelectPage "wizardTab" 1
FunctionEnd

Function un.btnGoPage1
	nsDui::PrePage "wizardTab"
FunctionEnd

Function un.ExitDUISetup
	nsDui::ExitDUISetup
FunctionEnd

#?ǡ???ǘ? 
Function un.onUninstall
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	;messagebox mb_ok $INSTDIR
	;IfFileExists "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk" 0 +2
	;ExecShell "taskbarunpin" "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk"
	
	;IfFileExists "$INSTDIR\run\${EXE_FILENAME}" 0 +2
	;ExecShell "taskbarunpin" "$INSTDIR\run\${EXE_FILENAME}"
	
	;IfFileExists "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk" 0 +2
	;LenovoDataRep::TaskbarPinWin10 "false" "$INSTDIR\run\" "$vDESKTOP_SHORTCUT_NAME.lnk"
	;IfFileExists "$INSTDIR\run\${EXE_FILENAME}" 0 +2
	;LenovoDataRep::TaskbarPinWin10 "false" "$INSTDIR\run\" "${EXE_FILENAME}"	
		
	ExecWait '"$INSTDIR\lmpservice.exe" /uninstall'
	
	nsProcess::_FindProcess "lmpservice.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmpservice.exe"
    ${EndIf}
	
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsDui::EnableButton "btnUnInstall" "false"
		nsDui::DuiMsb "$vun_exeisrunning" "lmp"
		nsDui::EnableButton "btnUnInstall" "true"
		goto InstallAbort
    ${EndIf}
	;messagebox mb_ok "$INSTDIR\lmpservice.exe /uninstall"
	;nsProcess::_FindProcess "LePopAdSe.exe"
	;Pop $R0
	;${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
	;	nsProcess::_KillProcess "LePopAdSe.exe"
    ;${EndIf}
	
	;nsProcess::_FindProcess "LeUpdate.exe"
	;Pop $R0
	;${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
	;	nsProcess::_KillProcess "LeUpdate.exe"
    ;${EndIf}
	
	nsProcess::_FindProcess "lmp.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmp.exe"
    ${EndIf}
	;nsDui::UnFA
	nsDui::NextPage "wizardTab"
	nsDui::EnableButton "btnClose" "false"
	nsDui::SetSliderRange "slrProgress" 0 100
	IntOp $UnInstallValue 0 + 10
    nsDui::GetCheckboxStatus "chkReserveData"
    Pop $0
	${If} $0 == 1
		call un.removeUserData
    ${EndIf}
    IntOp $UnInstallValue $UnInstallValue + 10
    nsDui::SetSliderValue "slrProgress" "progressLabel" $UnInstallValue
	IntOp $UnInstallValue $UnInstallValue + 10
    nsDui::SetSliderValue "slrProgress" "progressLabel" $UnInstallValue
	GetFunctionAddress $0 un.RemoveFiles
    BgWorker::CallAndWait
	InstallAbort:
FunctionEnd
;静默卸载
Function un.slientUninstall
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	
	;messagebox mb_ok "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk"
	
	IfFileExists "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk" 0 +2
	ExecShell "taskbarunpin" "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk"
	;messagebox mb_ok 1
	IfFileExists "$INSTDIR\run\${EXE_FILENAME}" 0 +2
	ExecShell "taskbarunpin" "$INSTDIR\run\${EXE_FILENAME}"
	;messagebox mb_ok 2
	IfFileExists "$INSTDIR\run\$vDESKTOP_SHORTCUT_NAME.lnk" 0 +2
	LenovoDataRep::TaskbarPinWin10 "false" "$INSTDIR\run\" "$vDESKTOP_SHORTCUT_NAME.lnk"
	;messagebox mb_ok 3
	IfFileExists "$INSTDIR\run\${EXE_FILENAME}" 0 +2
	;messagebox mb_ok 4
	LenovoDataRep::TaskbarPinWin10 "false" "$INSTDIR\run\" "${EXE_FILENAME}"
	;messagebox mb_ok 5
		
	ExecWait '"$INSTDIR\lmpservice.exe" /uninstall'
	
	nsProcess::_FindProcess "lmpservice.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmpservice.exe"
    ${EndIf}
	
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsDui::EnableButton "btnUnInstall" "false"
		nsDui::DuiMsb "$vun_exeisrunning" "lmp"
		nsDui::EnableButton "btnUnInstall" "true"
		goto InstallAbort
    ${EndIf}
	;messagebox mb_ok "$INSTDIR\lmpservice.exe /uninstall"

	
	nsProcess::_FindProcess "lmp.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmp.exe"
    ${EndIf}
	
	;nsDui::UnFA
	GetFunctionAddress $0 un.slientFile
    BgWorker::CallAndWait
	;messagebox mb_ok "remove end"
	InstallAbort:
	
FunctionEnd

Function un.slientFile
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	;messagebox mb_ok "$INSTDIR\LMPlayer.exe"
	;IfFileExists $INSTDIR\LMPlayer.exe removefile InstallAbort
	${Locate} "$INSTDIR" "/G=0 /M=*.*" "un.onSlientDeleteFileFound"
	
	
	Call un.DelShortcutContrlPanel	
	Call un.DelReg
	Sleep 5000
	Delete "$INSTDIR\${UINST_EXE_NAME}"
	RMDir /r "$INSTDIR"
	RMDir TEMP_USEDATA_DIR
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "53" "client" "$vPruduceVersion" $vChannelID

FunctionEnd

Function un.onSlientDeleteFileFound
    ; $R9    "path\name"
    ; $R8    "path"
    ; $R7    "name"
    ; $R6    "size"  ($R6 = "" if directory, $R6 = "0" if file with /S=)
	Delete "$R9"
	RMDir /r "$R9"
    RMDir "$R9"
	
FunctionEnd


Function un.DelReg
	DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\App Paths\LMPlayer"
	DeleteRegValue HKCU "Software\Microsoft\Windows\CurrentVersion\Run" "LMPlayer"
	DeleteRegKey HKCR "LMPlayer"
FunctionEnd
Function un.removeUserData
	SetShellVarContext current
	RMDir /r "${TEMP_USEDATA_DIR}"
FunctionEnd

Function un.RemoveFiles
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	removefile:
	${Locate} "$INSTDIR" "/G=0 /M=*.*" "un.onDeleteFileFound"
    Delete "$INSTDIR\${UINST_EXE_NAME}"
	StrCpy $InstallState "1"
	nsDui::SetSliderValue "slrProgress" 100
	${DirState} "$INSTDIR" $R0
	
	${If} $R0 == "0"
		ClearErrors
		;messagebox mb_ok $INSTDIR
        RMDir /r "$INSTDIR"
		IfErrors 0 +1
		RMDir /r "$INSTDIR"
    ${EndIf}
	RMDir /r "$INSTDIR"
	InstallAbort:
	nsDui::SetSliderValue "slrProgress" 100
    SetAutoClose true
	nsDui::NextPage "wizardTab"
	nsDui::EnableButton "btnClose" "true";
	Call un.DelShortcutContrlPanel	
	Call un.DelReg
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "53" "client" "$vPruduceVersion" $vChannelID
	
FunctionEnd

Function un.onDeleteFileFound
    ; $R9    "path\name"
    ; $R8    "path"
    ; $R7    "name"
    ; $R6    "size"  ($R6 = "" if directory, $R6 = "0" if file with /S=)
	Delete "$R9"
	RMDir /r "$R9"
    RMDir "$R9"
	
	IntOp $UnInstallValue $UnInstallValue + 2
	${If} $UnInstallValue > 100
		IntOp $UnInstallValue 100 + 0
		nsDui::SetSliderValue "slrProgress" "progressLabel" 100
	${Else}
		nsDui::SetSliderValue "slrProgress" "progressLabel" $UnInstallValue
		Sleep 100
	${EndIf}	
	undelete:
	Push "LocateNext"	
FunctionEnd

Function LangTransfer
    call GetCurrLangID
    StrCpy $vLangIDPath "$TEMP\skin\${CONFIGINI_NAME_PRE}$vLanguageId.ini"
    SetOutPath "$TEMP\skin"
    SetOverwrite on
    File /r info\*.*
    !insertmacro languagetrans
FunctionEnd

Function un.LangTransfer
    call un.GetCurrLangID
    StrCpy $vLangIDPath "$TEMP\skin\${CONFIGINI_NAME_PRE}$vLanguageId.ini"
    SetOutPath "$TEMP\skin"
    SetOverwrite on
    File /r info\*.*
    !insertmacro languagetrans
FunctionEnd

Function OnBtnLicenseNextClick
    nsDui::GetCheckboxStatus "Agree"
    Pop $0
    ${If} $0 == "1"
        call OnBtnInstall
    ${EndIf}
FunctionEnd

Function OnBtnAgree

	ExecShell "open" "https://appserver.lenovo.com.cn/Public/public_bottom/legal.shtml"

FunctionEnd
Function OnAgree
    nsDui::ItemEnabledByCheckbox "QuickInstall" "Agree"
	nsDui::ItemEnabledByCheckbox "DIYInstall" "Agree"
FunctionEnd

Function OnShowLicense
    nsDui::SetControlAttribute "ctrlbtnLicenseNext" "visible" "false"
    nsDui::SetControlAttribute "ctrlLicense" "visible" "true"
    
    nsDui::IsControlVisible "moreconfiginfo"
	Pop $0
	${If} $0 = 0        
		;pos="10,30,530,400"
		nsDui::SetControlAttribute "ctrlLicense" "pos" ${INNER_POS}		
	${Else}
		nsDui::SetControlAttribute "ctrlLicense" "pos" ${INNER_POS_MORE}
        nsDui::SetControlAttribute "moreconfiginfo" "visible" "false"
    ${EndIf}
FunctionEnd

Function OnbtnLicenseRead
    nsDui::SetControlAttribute "ctrlLicense" "visible" "false"
    nsDui::SetControlAttribute "ctrlbtnLicenseNext" "visible" "true"
    nsDui::IsControlVisible "btnHideMore"
	Pop $0
	${If} $0 = 1
        nsDui::SetControlAttribute "moreconfiginfo" "visible" "true"
    ${EndIf}
FunctionEnd

Function OnbtnLicentURL
	ExecShell open "https://guanjia.lenovo.com.cn/license.html"
FunctionEnd

Function OnbtnRegesterURL
	ExecShell open "https://www.lenovo.com.cn/public/cookies.html"
FunctionEnd

Function OnBtnInstall
	SetShellVarContext current
	ReadRegStr $INSTDIR HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	
	
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "${EXE_FILENAME}"
    ${EndIf}
	
	;nsProcess::_FindProcess "LePopAdSe.exe"
	;Pop $R0
	;${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
	;	nsProcess::_KillProcess "LePopAdSe.exe"
    ;${EndIf}
	
	;nsProcess::_FindProcess "LeUpdate.exe"
	;Pop $R0
	;${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
	;	nsProcess::_KillProcess "LeUpdate.exe"
    ;${EndIf}
	
	nsProcess::_FindProcess "LMPlayer.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "LMPlayer.exe"
    ${EndIf}
	
    ${If} $vCallOnce == 1
        goto InstallAbort
    ${EndIf}
    IntOp $vCallOnce 1 + 0
    nsDui::GetDirValue
    Pop $0
    StrCmp $0 "" InstallAbort 0
	StrCpy $1 $0 "" -1
	${if} $1 == "\"
	StrCpy $vInitPath "$0${PRODUCT_PUBLISHER}"
	;messagebox mb_ok "1"
	${else}
	StrCpy $vInitPath "$0\${PRODUCT_PUBLISHER}"
	;messagebox mb_ok "2"
	${endif}
    ;messagebox mb_ok $vInitPath
    nsDui::SelectPage "wizardTab" 2
	nsDui::EnableButton "btnClose" "false";
    nsDui::SetSliderRange "slrProgress" 0 100
    nsDui::SetSliderValue "slrProgress" "progressLabel" 0
    
    #??????Ɨ??????
    GetFunctionAddress $0 ExtractFunc
    BgWorker::CallAndWait
 
    ;Call CreateShortcut
    Call CreateUninstall
InstallAbort:
FunctionEnd

Function ExtractFunc
	;messagebox mb_ok $vInitPath
    SetOutPath $vInitPath
    nsDui::SetSliderProcessTimeValue "slrProgress" "progressLabel" 0 20 50
    File "app\LMPlayer.7z"
    nsDui::KillSliderProcessTimeValue
    nsDui::SetSliderValue "slrProgress" "progressLabel" 20
    GetFunctionAddress $R9 ExtractCallback
    Nsis7z::ExtractWithCallback "$vInitPath\LMPlayer.7z" $R9
    nsDui::NextPage "wizardTab"
	nsDui::EnableButton "btnClose" "true";
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "52" "安装成功" "$vPruduceVersion" $vChannelID
    Delete "$vInitPath\LMPlayer.7z"
FunctionEnd


Function ExtractCallback
    Pop $1
    Pop $2
    System::Int64Op $1 * 100
    Pop $3
    System::Int64Op $3 * 8
    Pop $4
    System::Int64Op $4 / 10
    Pop $3
    
    System::Int64Op $2 * 20
    Pop $5
    
    System::Int64Op $3 + $5
    Pop $3
    System::Int64Op $3 / $2
    Pop $0
    
    nsDui::SetSliderValue "slrProgress" "progressLabel" $0
FunctionEnd


Function OnExitDUISetup
    nsDui::ExitDUISetup
FunctionEnd

Function un.OnExitDUISetup
    nsDui::ExitDUISetup
FunctionEnd

Function OnBtnMin
    SendMessage $hInstallDlg ${WM_SYSCOMMAND} 0xF020 0
FunctionEnd

Function un.OnBtnMin
    SendMessage $hInstallDlg ${WM_SYSCOMMAND} 0xF020 0
FunctionEnd

Function OnBtnCancel
FunctionEnd

Function OnFinished
    ;ExecShell "open" "http://www.baidu.com"
    Exec "$vInitPath\${EXE_NAME}"
    Call OnExitDUISetup
FunctionEnd

Function OnBtnSelectDir
    nsDui::SelectInstallDir $vPROMPT_SELECT_INSTALL_DIR
    Pop $0
FunctionEnd

Function OnBtnDirPre
    nsDui::PrePage "wizardTab"
FunctionEnd


Function CreateShortcut
	SetShellVarContext current
	CreateShortCut "$DESKTOP\$vDESKTOP_SHORTCUT_NAME.lnk" "$vInitPath\${EXE_NAME}" "" "$vInitPath\Icon.ico"
	CreateDirectory "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR"
	CreateShortCut "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR\$vPROGRAM_SHUORTCUT_NAME.lnk" "$vInitPath\${EXE_NAME}"
	;CreateShortCut "$vInitPath\run\$vDESKTOP_SHORTCUT_NAME.lnk" "$vInitPath\run\${EXE_FILENAME}"
	;Version::IsWindows7
	;Pop $0
	;${if} $0 == 1
		;ExecShell "taskbarunpin" "$DESKTOP\$vDESKTOP_SHORTCUT_NAME.lnk"
		;messagebox mb_ok "win7"
		ExecShell "taskbarpin" "$vInitPath\run\$vDESKTOP_SHORTCUT_NAME.lnk"
	;${else}
		;messagebox mb_ok "win10"
		;ExecWait '"$INSTDIR\run\taskbarpin.exe" unpin $INSTDIR\run\${EXE_FILENAME}'
	;	LenovoDataRep::TaskbarPinWin10 "true" "$vInitPath\run\" "${EXE_FILENAME}"
	;${Endif}
FunctionEnd



Function CreateUninstall
	SetShellVarContext current
	CreateDirectory "$vInitPath\run"
	WriteUninstaller "$vInitPath\run\${UINST_EXE_NAME}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "DisplayIcon" "$vInitPath\${EXE_NAME}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "DisplayName" "$vDESKTOP_SHORTCUT_NAME"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "UninstallString" "$vInitPath\run\uninst.exe"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "Publisher" "$vDESKTOP_SHORTCUT_NAME"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "URLInfoAbout" "https://www.lenovo.com.cn/"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation" "$vInitPath"
	WriteRegDWORD HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "EstimatedSize" 200000
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "ctext" "LMPlayer"
	WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "curl" "https://www.lenovo.com.cn/"

FunctionEnd


Function un.DelShortcutContrlPanel
    SetShellVarContext current
	
    Delete "$DESKTOP\$vDESKTOP_SHORTCUT_NAME.lnk"
	Delete "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR\$vPROGRAM_SHUORTCUT_NAME.lnk"
    Delete "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR\$vCTROLPANEL_UNINSTALL_NAME.lnk"
    RMDir "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR"
    
	SetShellVarContext All
	
	Delete "$DESKTOP\$vDESKTOP_SHORTCUT_NAME.lnk"
	Delete "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR\$vPROGRAM_SHUORTCUT_NAME.lnk"
    Delete "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR\$vCTROLPANEL_UNINSTALL_NAME.lnk"
    RMDir "$SMPROGRAMS\Lenovo\$vSHORTCUTSUBDIR"
	
    ;${StdUtils.InvokeShellVerb} $0 "$vInitPath" "${EXE_NAME}" ${StdUtils.Const.ShellVerb.UnpinFromTaskbar}
    ;messagebox mb_ok "$SMPROGRAMS\$vSHORTCUTSUBDIR"
	
    SetShellVarContext current
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}"
	DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}"
	;DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Run\LMPlayer"
	
FunctionEnd

Function .onInstFailed
    MessageBox MB_ICONQUESTION|MB_YESNO "Ǔ???Ȣ" /SD IDYES IDYES +2 IDNO +1
FunctionEnd



