######################################################################
# Automatically generated by qmake (2.01a) ?? ?? 23 19:27:34 2014
######################################################################

TEMPLATE = lib
TARGET = imgdecoder-gdip

!LIB_ALL:!COM_LIB{
	RC_FILE += imgdecoder-gdip.rc
	CONFIG += dll
}
else{
	CONFIG += staticlib
}


DEPENDPATH += .
INCLUDEPATH += . \
			   ../png \
			   ../zlib \
			   ../../utilities/include \
			   ../../soui/include \

dir = ../..
include($$dir/common.pri)

# Input
HEADERS += imgdecoder-gdip.h
SOURCES += imgdecoder-gdip.cpp
