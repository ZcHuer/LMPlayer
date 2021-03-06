
# ======================LMPlayer install program==============================
!define PRODUCT_NAME           "LMPlayer" 
!define EXE_NAME               "LMPlayer.exe" 
!define EXE_FILENAME 		   "LMPlayer.exe"
!define INST_SUBDIR            "" 
!define PRODUCT_VERSION        "2.0.2.2"
!define PRODUCT_PUBLISHER      "LMPlayer" 
!define PRODUCT_LEGAL          "Copyright (C) 1998-2020 Lenovo, All Rights Reserved" 
!define CONFIGINI_NAME_PRE     "info"
!define LICENSETXT_FILE_PRE       "license" 
!define SKINFILEXML_PRE           "install" 
!define SKINFILEXML_UNINST_PRE           "uninstall" 
!define PCODE                  "" 
!define TIME_COMPILE_INSTALLER "2020-2-11"


!define WIDTH 600 
!define HEIGHT 340 
!define WIDTH_MORE 600 
!define HEIGHT_MORE 440 
!define INNER_POS "10,30,590,335" 
!define INNER_POS_MORE  "10,30,590,435"

!define UINST_EXE_NAME "uninst.exe" 
!define TEMP_USEDATA_DIR "$LOCALAPPDATA\LMPlayerTMP"
!define GAME_INSTALL_DIR ""
!define /date PRODUCT_DATE %Y%m%d

# ==================== 设置NSIS 压缩方式================================
;SetCompressor /SOLID LZMA
;SetCompressor zlib

# =====================NSIS头文件 =============================
!include "LogicLib.nsh"
!include "nsDialogs.nsh"
!include "include\common.nsh"
!include "StdUtils.nsh"

# =====================安装包属性信息=============================

VIProductVersion                    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductVersion"    "${PRODUCT_VERSION}"
VIAddVersionKey "ProductName"       "${PRODUCT_NAME}"
VIAddVersionKey "CompanyName"       "${PRODUCT_PUBLISHER}"
VIAddVersionKey "FileVersion"       "${PRODUCT_VERSION}"
VIAddVersionKey "InternalName"      "${EXE_NAME}"
VIAddVersionKey "FileDescription"   "${PRODUCT_NAME}"
VIAddVersionKey "LegalCopyright"    "${PRODUCT_LEGAL}"

# =====================变量定义==============================
Var vPROMPT_SELECT_INSTALL_DIR
Var vDESKTOP_SHORTCUT_NAME
Var vSHORTCUTSUBDIR
Var vPROGRAM_SHUORTCUT_NAME
Var vCTROLPANEL_UNINSTALL_NAME
Var vun_exeisrunning

Var vLanguageId
Var vLangIDPath
Var vVersionPath

Var vServerUrl
Var vPruduceVersion
Var vConifgPath
Var vChannelID
Var vParamA

!define PRODUCT_UNIST_CONTRL_PATH "${PRODUCT_NAME}${PCODE}"
!define INSTALLNAME "LMPlayer_${PCODE}${PRODUCT_DATE}.exe"


Name "${PRODUCT_NAME}"

OutFile "LMPlayer_${PRODUCT_VERSION}.exe"

InstallDir "D:\${PRODUCT_PUBLISHER}"


!macro serverCfg
	StrCpy $vServerUrl "https://collect.vgs.lenovo.com.cn/"
	StrCpy $vPruduceVersion "${PRODUCT_VERSION}"
!macroend
!macro ReadVersionCfg
	SetShellVarContext current
	SetOutPath "$TEMP\lmpskin"
    File /r Config.ini
	StrCpy $vConifgPath "$TEMP\lmpskin\Config.ini"
	ReadINIStr $vPruduceVersion $vConifgPath Update CurVer
	ReadINIStr $vChannelID $vConifgPath Channel ChannelID
!macroend

!macro languagetrans
ReadINIStr $vPROMPT_SELECT_INSTALL_DIR $vLangIDPath Installation PROMPT_SELECT_INSTALL_DIR
ReadINIStr $vSHORTCUTSUBDIR $vLangIDPath Installation SHORTCUTSUBDIR
ReadINIStr $vDESKTOP_SHORTCUT_NAME $vLangIDPath Installation DESKTOP_SHORTCUT_NAME
ReadINIStr $vPROGRAM_SHUORTCUT_NAME $vLangIDPath Installation PROGRAM_SHUORTCUT_NAME
ReadINIStr $vCTROLPANEL_UNINSTALL_NAME $vLangIDPath Installation CTROLPANEL_UNINSTALL_NAME
ReadINIStr $vun_exeisrunning $vLangIDPath Installation un_exeisrunning
!macroend

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

Function .onInit
	InitPluginsDir
	System::Call 'kernel32::CreateMutex(i 0, i 0, w "LMPlayerinstaller") i .r1 ?e'
	Pop $R0
	StrCmp $R0 0 +2
    Abort
FunctionEnd

Section .init
	!insertmacro serverCfg
	!insertmacro ReadVersionCfg
	IfSilent 0 +2
	push "false"
	call installSlient 
	
SectionEnd

Function .onInstFailed
    MessageBox MB_ICONQUESTION|MB_YESNO "Ǔ???Ȣ" /SD IDYES IDYES +2 IDNO +1
FunctionEnd

# RequestExecutionLevel none|user|highest|admin
RequestExecutionLevel admin

Icon              "image\logo.ico"
UninstallIcon     "image\unlogo.ico"

Page custom DUIPage
UninstPage custom un.DUIPage

# ======================= DUILI=========================
Var hInstallDlg		;DUI 窗口句柄
Var vInitPath 		;安装路径

AllowSkipFiles on 

;检查本地安装过的版本
Function CheckOldVersion
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		goto InstallAbort
    ${EndIf}
	;messagebox mb_ok $INSTDIR
	${if} $INSTDIR != ""	
		ExecWait '"$INSTDIR\run\uninst.exe" /S _?=$INSTDIR'
		;ExecWait '"$INSTDIR\uninst.exe" /S _?=$INSTDIR'
	${endif}
	InstallAbort:
FunctionEnd

Function DUIPage

	${GetOptions} "/A" "/A"  $R0
	IfErrors 0 +2
	Goto duiinstall
	push "true"
	Call installSlient
	Goto InstallAbort
duiinstall:	
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	${if} $INSTDIR != ""
		StrCpy $vInitPath "$INSTDIR" -9
	${else} 
		StrCpy $vInitPath "$APPDATA"
	${endif}
	InitPluginsDir
	;C:\Users\Administrator\AppData\Roaming\LMPlayerCenter
	call LangTransfer
	!insertmacro ReadVersionCfg
	
	
	nsSCM::Stop /NOUNLOAD "LocalSystemNetworkRestricted"
	pop $R0
    SetOutPath "$TEMP\lmpskin"
    File /r skin\*.*
    nsDui::InitDUISetup "install" "$TEMP\lmpskin" "${SKINFILEXML_PRE}$vLanguageId.xml" "$vInitPath\${LICENSETXT_FILE_PRE}$vLanguageId.txt" "wizardTab" 0 "showPic" 1500
    Pop $hInstallDlg
	nsDui::SetDirValue "$vInitPath"
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
	;;
	nsDui::FindControl "DIYInstallBack"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnDIYInstallBack
        nsDui::OnControlBindNSISScript "DIYInstallBack" $0
    ${EndIf}
	
	;选择目录
	nsDui::FindControl "Agree"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnAgree
        nsDui::OnControlBindNSISScript "Agree" $0
    ${EndIf}
	
	
	;选择目录
	nsDui::FindControl "ShowPic0"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnShowPic0
        nsDui::OnControlBindNSISScript "ShowPic0" $0
    ${EndIf}
	
	nsDui::FindControl "ShowPic1"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnShowPic1
        nsDui::OnControlBindNSISScript "ShowPic1" $0
    ${EndIf}
	
	nsDui::FindControl "ShowPic2"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnShowPic2
        nsDui::OnControlBindNSISScript "ShowPic2" $0
    ${EndIf}
	
	nsDui::FindControl "Agree2"
    Pop $0
    ${If} $0 == 0
        GetFunctionAddress $0 OnAgree2
        nsDui::OnControlBindNSISScript "Agree2" $0
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
    GetFunctionAddress $0 OnBtnInstall
    nsDui::ShowPage "" $0 2000
InstallAbort:
FunctionEnd

;安装界面轮播图不需要点击
Function OnShowPic0
	;nsDui::LunchApp 0 "$vInitPath\${EXE_NAME}"
FunctionEnd
Function OnShowPic1
	;nsDui::LunchApp 1 "$vInitPath\${EXE_NAME}"
FunctionEnd
Function OnShowPic2
	;nsDui::LunchApp 2 "$vInitPath\${EXE_NAME}"
FunctionEnd

;显示选择安装路径
Function OnDIYInstallBack
	nsDui::SetWindowSize $hInstallDlg 600 430
	nsDui::PrePage "wizardTab"
FunctionEnd
;返回
Function OnbtnBack
	nsDui::PrePage "wizardTab"
FunctionEnd
Function OnQuickInstall
	call OnBtnInstall
FunctionEnd

;添加防火墙规则
Function addFirewall
	;ExecDos::exec 'netsh advfirewall firewall add rule name="$EXEFILE" dir=in program="$EXEPATH" action=allow'
	;ExecDos::exec 'netsh advfirewall firewall add rule name="$EXEFILE" dir=out program="$EXEPATH" action=allow'
FunctionEnd

Function OnDIYInstall
	nsDui::NextPage "wizardTab"
FunctionEnd

Function installSlient
	pop $vParamA
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		goto InstallAbort
    ${EndIf}
	;messagebox mb_ok $INSTDIR
	${if} $INSTDIR != ""	
		ExecWait '"$INSTDIR\run\uninst.exe" /S _?=$INSTDIR'
		;ExecWait '"$INSTDIR\uninst.exe" /S _?=$INSTDIR'
	${endif}
	
	pop $R2
	call LangTransfer	
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	
	
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "${EXE_FILENAME}"
    ${EndIf}
	;messagebox mb_ok "$INSTDIR\lmpservice.exe /uninstall"
	ExecWait "$INSTDIR\lmpservice.exe /uninstall"
	nsProcess::_FindProcess "lmpservice.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmpservice.exe"
    ${EndIf}	
	
	nsProcess::_FindProcess "LMPlayer.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "LMPlayer.exe"
    ${EndIf}
	
	nsProcess::_FindProcess "lmp.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmp.exe"
    ${EndIf}
	
	SetShellVarContext current
	;messagebox mb_ok $APPDATA 
	StrCpy $vInitPath "$APPDATA\${PRODUCT_PUBLISHER}"
	ExecWait '"$vInitPath\lmpservice.exe" /uninstall'
	nsProcess::_FindProcess "lmpservice.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmpservice.exe"
    ${EndIf}
	
	SetOutPath $vInitPath
	File "app\LMPlayer.7z"
    Nsis7z::Extract "$vInitPath\LMPlayer.7z"
	;messagebox mb_ok $vParamA
    ${if} $vParamA != "true"
		Call SlientInstallFinish
	${endif}
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\LMPlayer" "LMPlayer" "$vInitPath\${EXE_NAME}"	
	Call CreateUninstall
	ExecWait '"$vInitPath\lmpservice.exe" /install'
	;执行服务安装
	Delete "$vInitPath\LMPlayer.7z"
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "52" "静默安装成功" "$vPruduceVersion" $vChannelID
InstallAbort:
FunctionEnd

Function un.DUIPage
    
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

Function LangTransfer
    call GetCurrLangID
    StrCpy $vLangIDPath "$TEMP\lmpskin\${CONFIGINI_NAME_PRE}$vLanguageId.ini"
	StrCpy $vVersionPath "$TEMP\lmpskin\Config.ini"
    SetOutPath "$TEMP\lmpskin"
    SetOverwrite on
    File /r info\*.*
	file /r Config.ini
    !insertmacro languagetrans
FunctionEnd


Function OnBtnAgree
	ExecShell "open" "https://appserver.lenovo.com.cn/Public/public_bottom/legal.shtml"
FunctionEnd
Function OnAgree
    nsDui::ItemEnabledByCheckbox "QuickInstall" "Agree"
	nsDui::ItemEnabledByCheckbox "DIYInstall" "Agree"
FunctionEnd

Function OnAgree2
    nsDui::ItemEnabledByCheckbox "btnInstall" "Agree2"
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



Function OnShowMorwe
    nsDui::SetControlAttribute "moreconfiginfo" "visible" "true"
    nsDui::SetControlAttribute "btnShowMore" "visible" "false"
    nsDui::SetControlAttribute "btnHideMore" "visible" "true"
    ;?????? ??ǹ???Ȭ????

	nsDui::SetWindowSize $hInstallDlg ${WIDTH_MORE} ${HEIGHT_MORE}
FunctionEnd

Function OnBtnInstall
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${if} $INSTDIR == ""
		ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	${endif}
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		goto InstallAbort
    ${EndIf}
	
	${if} $INSTDIR != ""
		ExecWait '"$INSTDIR\run\uninst.exe" /S _?=$INSTDIR'
		Delete "$INSTDIR\${UINST_EXE_NAME}"
		RMDir /r "$INSTDIR"
	${endif}
	
	SetShellVarContext current
	ReadRegStr $INSTDIR HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation"
	
	nsProcess::_FindProcess "${EXE_FILENAME}"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "${EXE_FILENAME}"
    ${EndIf}
		
	;messagebox mb_ok "$INSTDIR\lmpservice.exe /uninstall"
	ExecWait "$INSTDIR\lmpservice.exe /uninstall"
	
	nsProcess::_FindProcess "lmpservice.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmpservice.exe"
    ${EndIf}
		
	nsProcess::_FindProcess "LMPlayer.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "LMPlayer.exe"
    ${EndIf}
	
	nsProcess::_FindProcess "lmp.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmp.exe"
    ${EndIf}
	
    nsDui::GetDirValue
    Pop $0
    StrCmp $0 "" InstallAbort 0
    StrCpy $1 $0 "" -1
	${if} $1 == "\"
	StrCpy $vInitPath "$0${PRODUCT_PUBLISHER}"
	;messagebox mb_ok "1"
	${else}
	StrCpy $vInitPath "$0\${PRODUCT_PUBLISHER}"
	
	ExecWait "$vInitPath\lmpservice.exe /uninstall"
	
	nsProcess::_FindProcess "lmpservice.exe"
	Pop $R0
	${If} $R0 == 0
        ;MessageBox MB_ICONINFORMATION|MB_OK "$vun_exeisrunning" /SD IDOK
		nsProcess::_KillProcess "lmpservice.exe"
    ${EndIf}
	;messagebox mb_ok "2"
	${endif}
    ;messagebox mb_ok $vInitPath
    nsDui::SelectPage "wizardTab" 2
	nsDui::EnableButton "btnClose" "false";
    nsDui::SetSliderRange "slrProgress" 0 100
    nsDui::SetSliderValue "slrProgress" "progressLabel" 0
    ;关闭兼容助手
	WriteRegStr HKLM "SYSTEM\CurrentControlSet\services\PcaSvc" "DisplayName" "Program Compatibility Assistant Service"
	WriteRegDWORD HKLM "SYSTEM\CurrentControlSet\services\PcaSvc" "Start" 00000004
    #??????Ɨ??????
    GetFunctionAddress $0 ExtractFunc
    BgWorker::CallAndWait
 
	;执行服务安装
	;messagebox mb_ok "$vInitPath\\lmpservice.exe /install"
	ExecWait '"$vInitPath\lmpservice.exe" /install'
    Delete "$vInitPath\LMPlayer.7z"
	Call CreateShortcutFun	
    Call CreateUninstall
	nsDui::NextPage "wizardTab"
	nsDui::EnableButton "btnClose" "true"
	!insertmacro ReadVersionCfg
	LenovoDataRep::ReportLePlayerData "$vServerUrl" "52" "安装成功" "$vPruduceVersion" $vChannelID
   
InstallAbort:
FunctionEnd

Function ExtractFunc
	;messagebox mb_ok $vInitPath
    SetOutPath $vInitPath
    nsDui::SetSliderProcessTimeValue "slrProgress" "progressLabel" 0 20 50
    File "app\LMPlayer.7z"
	
    nsDui::KillSliderProcessTimeValue
    nsDui::SetSliderValue "slrProgress" "progressLabel" 20
    ;nsDui::SetBgImage "vrlDetail" "install_s3.png"
    GetFunctionAddress $R9 ExtractCallback
    Nsis7z::ExtractWithCallback "$vInitPath\LMPlayer.7z" $R9
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
    Exec "$vInitPath\${EXE_NAME} -install"
    Call OnExitDUISetup
FunctionEnd

Function OnBtnSelectDir
    nsDui::SelectInstallDir $vPROMPT_SELECT_INSTALL_DIR
    Pop $0
FunctionEnd

Function OnBtnDirPre
    nsDui::PrePage "wizardTab"
FunctionEnd


# =========================静默安装包完成===============================
Function SlientInstallFinish
  SetShellVarContext all
  strcpy $0 $APPDATA 3
  ;messagebox mb_ok $vInitPath
  CreateDirectory "$SMPROGRAMS\$vSHORTCUTSUBDIR"
  CreateShortCut "$DESKTOP\$vDESKTOP_SHORTCUT_NAME.lnk" "$vInitPath\${EXE_NAME}" "-desktop.lnk"
  ;MessageBox mb_ok "$SMPROGRAMS\$vSHORTCUTSUBDIR"
  CreateShortCut "$SMPROGRAMS\$vSHORTCUTSUBDIR\$vPROGRAM_SHUORTCUT_NAME.lnk" "$vInitPath\${EXE_NAME}" "-startMenu.lnk"
  CreateShortCut "$vInitPath\run\$vDESKTOP_SHORTCUT_NAME.lnk" "$vInitPath\run\${EXE_FILENAME}" "-taskbar.lnk"
  Version::IsWindows7
	#obtain value
	Pop $0
	;messagebox mb_ok $0
	${if} $0 == 1		
		ExecShell "taskbarpin" "$vInitPath\run\$vDESKTOP_SHORTCUT_NAME.lnk"
	${else}		
		LenovoDataRep::TaskbarPinWin10 "true" "$vInitPath\run\" "$vDESKTOP_SHORTCUT_NAME.lnk"
	${Endif}
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\run" "LMPlayer" "$vInitPath\${EXE_NAME} /AutoRun"
FunctionEnd

Function CreateShortcutFun
  SetShellVarContext all
  CreateShortCut "$DESKTOP\$vDESKTOP_SHORTCUT_NAME.lnk" "$vInitPath\${EXE_NAME}" "-desktop.lnk"
  CreateShortCut "$vInitPath\run\$vDESKTOP_SHORTCUT_NAME.lnk" "$vInitPath\run\${EXE_FILENAME}" "-taskbar.lnk"
  CreateDirectory "$SMPROGRAMS\$vSHORTCUTSUBDIR"
  CreateShortCut "$SMPROGRAMS\$vSHORTCUTSUBDIR\$vPROGRAM_SHUORTCUT_NAME.lnk" "$vInitPath\${EXE_NAME}" "-startMenu.lnk"
  Version::IsWindows7
  Pop $0
  ${if} $0 == 1
  	ExecShell "taskbarpin" "$vInitPath\run\$vDESKTOP_SHORTCUT_NAME.lnk"
  ${else}
  	LenovoDataRep::TaskbarPinWin10 "true" "$vInitPath\run" "$vDESKTOP_SHORTCUT_NAME.lnk"
  ${Endif}
  SetShellVarContext current
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\App Paths\LMPlayer" "LMPlayer" "$vInitPath\${EXE_NAME}"  
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "LMPlayer" "$vInitPath\${EXE_NAME} /AutoRun"
  ;生成协议
  WriteRegStr HKCR "LMPlayer" "" "LMPlayer"
  WriteRegStr HKCR "LMPlayer" "URL Protocol" "LMPlayer"
  WriteRegStr HKCR "LMPlayer\DefaultIcon" "" "$vInitPath\${EXE_NAME},1"
  WriteRegStr HKCR "LMPlayer\shell" "" ""
  WriteRegStr HKCR "LMPlayer\shell\open" "" ""
  WriteRegStr HKCR "LMPlayer\shell\open\command" "" "$vInitPath\${EXE_NAME} %1"
FunctionEnd


;生成安装包卸载程序
Function CreateUninstall

	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "DisplayIcon" "$vInitPath\${EXE_NAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "DisplayName" "$vDESKTOP_SHORTCUT_NAME"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "DisplayVersion" "${PRODUCT_VERSION}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "UninstallString" "$vInitPath\run\uninst.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "Publisher" "$vDESKTOP_SHORTCUT_NAME"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "URLInfoAbout" "https://www.lenovo.com.cn/"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "InstallLocation" "$vInitPath"
	WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "EstimatedSize" 32000
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "ctext" "LMPlayer"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_UNIST_CONTRL_PATH}" "curl" "https://www.lenovo.com.cn/"
FunctionEnd

