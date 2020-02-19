@ECHO OFF
IF %PROCESSOR_ARCHITECTURE% == x86 (
	 IF DEFINED PROCESSOR_ARCHITEW6432 ( SET reg3264= /reg:64 ) else ( SET reg3264= /reg:32 )
) else ( SET reg3264= /reg:64 )

echo "Add reg"
REG ADD "HKCU\SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\LenovoVideo" /f /ve /d "%~dp0LeLaunch.exe" /t REG_SZ
REG ADD "HKLM\SOFTWARE\Lenovo\PcManager\LePlayer" /f /v InstallPath /t REG_SZ /d %~dp0 /reg:32
SET exitcode = %errorlevel%
ECHO "Infor : Add reg result : %errorlevel%  %reg3264%"
del %0
exit %exitcode%

