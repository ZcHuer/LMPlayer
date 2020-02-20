copy /y cef_binary\\release\\*.dll D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\dll\\release
copy /y cef_binary\\release\\*.bin D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\dll\\release
copy /y cef_binary\\release\\*.exe D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\dll\\release
copy /y cef_binary\\Resources\\locales\\*.pak D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\dll\\release\\locales
copy /y cef_binary\\Resources\\*.pak D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\dll\\release
copy /y cef_binary\\Resources\\*.dat D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\dll\\release

copy /y cef_binary\\release\\*.lib D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\lib\\release
copy /y cef_binary\\release\\*.lib D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\lib\\debug
copy /y cef_binary\\libcef_dll_wrapper\\debug\\libcef_dll_wrapper.lib D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\lib\\debug
copy /y cef_binary\\libcef_dll_wrapper\\release\\libcef_dll_wrapper.lib D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\lib\\release

copy /y cef_symbols\\*.pdb D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\pdb

copy /y cef_binary\\release\\*.dll D:\\51talk\\ACTalk\\bin\\Debug
copy /y cef_binary\\release\\*.bin D:\\51talk\\ACTalk\\bin\\Debug
copy /y cef_binary\\release\\*.exe D:\\51talk\\ACTalk\\bin\\Debug
copy /y cef_binary\\Resources\\locales\\*.pak D:\\51talk\\ACTalk\\bin\\Debug\\locales
copy /y cef_binary\\Resources\\*.pak D:\\51talk\\ACTalk\\bin\\Debug
copy /y cef_binary\\Resources\\*.dat D:\\51talk\\ACTalk\\bin\\Debug
copy /y cef_symbols\\*.pdb D:\\51talk\\ACTalk\\bin\\Debug

xcopy /y /S cef_binary\\include D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\src\\include
xcopy /y /S cef_binary\\libcef_dll D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\src\\libcef_dll

call "C:\\Program Files (x86)\Microsoft Visual Studio 14.0\\Common7\\Tools\\VsDevCmd.bat"
devenv "3.2704.1416\\src\\cef.sln" /Rebuild "debug"
devenv "3.2704.1416\\src\\cef.sln" /Rebuild "release"

copy /y 3.2704.1416\\src\\libcef_dll_wrapper\\debug\\libcef_dll_wrapper.lib D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\lib\\debug
copy /y 3.2704.1416\\src\\libcef_dll_wrapper\\release\\libcef_dll_wrapper.lib D:\\51talk\\ACTalk\\3rdparty\\cef3\\3.2704.1416\\lib\\release
pause