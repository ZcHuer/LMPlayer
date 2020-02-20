echo off
cls
echo  CreateSetup.bat
echo  0:测试包 (add rebuild)
echo  new:正式包(add NewVersion)  
echo  1:sign LePlayer       
echo  2:create 7z and create installer    
echo  3:installer signed
echo  4:SVN 提交
echo  5:SVN 添加并提交       
echo  请选择0-4 by xdb 20181022      
@set svn_bin=C:\Program Files\TortoiseSVN\bin
set /p a=请选择0~5：
echo %a%
if "%a%"=="0" goto all
if "%a%"=="new" goto allNew
if "%a%"=="1" goto p1
if "%a%"=="2" goto p2
if "%a%"=="3" goto p3
if "%a%"=="4" goto p4
if "%a%"=="5" goto p5

:all
call :1
call :2
call :3
call :4 %svn_bin%
goto finish

:allNew
call :1
call :2
call :3
call :5 %svn_bin%
goto finish

:p1
call :1
goto finish
:p2
call :2
goto finish
:p3
call :3
goto finish
:p4
call :4 %svn_bin%
goto finish
:p5
call :5 %svn_bin%
goto finish

pause

rem 签名散文件
:1
echo **********************Sign Single document**********************
%~dp0signtool sign /f lenovogame.pfx /p 800300 /t http://timestamp.verisign.com/scripts/timstamp.dll ./LePlayer/LePlayerService.exe ./LePlayer/LeLaunch.exe ./LePlayer/run/swscale-5.dll ./LePlayer/run/swresample-3.dll ./LePlayer/run/SDL2.dll ./LePlayer/run/LeUpdate.exe ./LePlayer/run/LePlayer.exe ./LePlayer/run/Player.dll ./LePlayer/run/LeDaemonWork.dll ./LePlayer/run/gma.dll ./LePlayer/run/FunshionService.dll ./LePlayer/run/funotc.dll ./LePlayer/run/avutil-56.dll ./LePlayer/run/avformat-58.dll ./LePlayer/run/avcodec-58.dll ./LePlayer/run/CEF/widevinecdmadapter.dll ./LePlayer/run/CEF/libGLESv2.dll ./LePlayer/run/CEF/libEGL.dll ./LePlayer/run/CEF/libcef.dll ./LePlayer/run/CEF/LeCef.dll ./LePlayer/run/CEF/d3dcompiler_47.dll ./LePlayer/run/CEF/d3dcompiler_43.dll ./LePlayer/run/uninst.exe ./LePlayer/run/Update.dll ./LePlayer/run/LeRepair.exe ./installer/app/server/LePlayerService.exe ./LePlayer/run/LePopAdSe.exe
goto:eof

rem 创建安装包
:2
echo **********************create 7z and create installer************
%~dp07-Zip/7z.exe a -t7z %~dp0installer/app/LePlayer.7z "%~dp0LePlayer/*" -r -mx=9 -m0=LZMA2 -ms=10m -mf=on -mhc=on -mmt=on
%~dp0installer\NSIS\NSIS\makensis.exe %~dp0installer/LePlayerInstaller.nsi

goto:eof
rem 签名安装包
:3
echo **********************installer signed ************************
%~dp0signtool sign /f lenovogame.pfx /p 800300 /t http://timestamp.verisign.com/scripts/timstamp.dll %~dp0LePlaye_Setup\LePlayer_1.2.0.8.exe
goto:eof

rem SVN上传只是commit 不会添加
:4
set /p ver=请输入安装包文件版本：
echo "%~dp0installer\LePlayer_%ver%.exe"
move "%~dp0installer\LePlayer_%ver%.exe" "%~dp0\LePlaye_Setup"
%~dp0signtool sign /f lenovogame.pfx /p 800300 /t http://timestamp.verisign.com/scripts/timstamp.dll %~dp0installer\LePlayer_%ver%.exe
set /p msg=请输入SVN提交日志：
"%ProgramFiles%\TortoiseSVN\bin\TortoiseProc.exe" /command:commit /path:"%~dp0" /logmsg:"%msg%" /closeonend:4
rem 参数错误
goto:eof

rem 添加新的版本，添加SVN并且commit
:5
set /p ver=请输入安装包文件版本：
echo "%~dp0installer\LePlayer_%ver%.exe"
move "%~dp0installer\LePlayer_%ver%.exe" "%~dp0\LePlaye_Setup"
%~dp0signtool sign /f lenovogame.pfx /p 800300 /t http://timestamp.verisign.com/scripts/timstamp.dll "%~dp0\LePlaye_Setup/LePlayer_%ver%.exe"
"%ProgramFiles%\TortoiseSVN\bin\TortoiseProc.exe" /command:add /path:"%~dp0/LePlaye_Setup/LePlayer_%ver%.exe" /logmsg:"add %ver%" /closeonend:4
"%ProgramFiles%\TortoiseSVN\bin\TortoiseProc.exe" /command:commit /path:"%~dp0" /logmsg:"add %ver%" /closeonend:4
goto:eof
:ch
echo 参数错误
goto:eof



:finish
echo "exec finished"
goto:eof