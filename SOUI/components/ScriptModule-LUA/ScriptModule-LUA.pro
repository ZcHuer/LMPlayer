######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 24 16:29:36 2014
######################################################################

TEMPLATE = lib
TARGET = scriptmodule-lua

!LIB_ALL:!COM_LIB{
	RC_FILE += src/ScriptModule-Lua.rc
	CONFIG += dll
}
else{
	CONFIG += staticlib
}

DEPENDPATH += . exports
INCLUDEPATH += . \
			   ./src \
			   ../../third-part/lua-52/src \
			   ../../soui/include \
			   ../../utilities/include \


dir = ../..
include($$dir/common.pri)


CONFIG(debug,debug|release){
	LIBS += lua-52d.lib souid.lib utilitiesd.lib
}
else{
	LIBS += lua-52.lib soui.lib utilities.lib
}

PRECOMPILED_HEADER = src/stdafx.h

# Input
HEADERS += src/require.h \
           src/ScriptModule-Lua.h \
           src/exports/exp_app.h \
           src/exports/exp_Basic.h \
           src/exports/exp_eventArgs.h \
           src/exports/exp_hostwnd.h \
           src/exports/exp_msgbox.h \
           src/exports/exp_pugixml.h \
           src/exports/exp_ResProvider.h \
           src/exports/exp_ScriptModule.h \
           src/exports/exp_strcpcvt.h \
           src/exports/exp_string.h \
           src/exports/exp_Window.h \
           src/exports/exp_Object.h \
           src/exports/exp_ctrls.h \
           lua_tinker/lua_tinker.h

SOURCES += src/ScriptModule-Lua.cpp \
           src/exports/exp_soui.cpp \
           lua_tinker/lua_tinker.cpp

