######################################################################
# Automatically generated by qmake (1.07a) Thu Apr 14 21:14:38 2005
######################################################################

TEMPLATE = lib
INCLUDEPATH += . ../include/

# Input
HEADERS += TelnetFilter.h
SOURCES += TelnetFilter.cpp

DESTDIR = ../lib/
LIBS += -L../lib -lCommon
OBJECTS_DIR	= obj
MOC_DIR		= moc
CONFIG += debug thread qt opengl warn_on
win32 {
	CONFIG	+= console
}