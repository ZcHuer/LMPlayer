@echo off
set regpath=HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment
set evname=SOUIPATH
set SOUIPATH=%cb%
reg add "%regpath%" /v %evname% /d %SOUIPATH% /f