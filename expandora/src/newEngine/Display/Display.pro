######################################################################
# Automatically generated by qmake (1.07a) Wed Apr 13 17:57:43 2005
######################################################################

TEMPLATE = lib
INCLUDEPATH += . ../include/


# Input
HEADERS += Display.h MainWindow.h Terrain.h CachedRoom.h
SOURCES += Display.cpp MainWindow.cpp Terrain.cpp CachedRoom.cpp

DESTDIR = ../lib/
LIBS += -L../lib -lCommon
OBJECTS_DIR	= obj
MOC_DIR		= moc
CONFIG += debug thread qt warn_on
QT += opengl
win32 {
	CONFIG	+= console
}
