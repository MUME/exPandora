

TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

CONFIG		+= qt opengl warn_on thread

QT += xml opengl gui

win32 {
	CONFIG	+= console
}

FORMS +=	configedit.ui
FORMS +=	patterndialog.ui
FORMS +=        roomedit.ui

HEADERS		+=configurator.h
HEADERS		+=croom.h
HEADERS		+=defines.h
HEADERS		+=dispatch.h
HEADERS		+=engine.h
HEADERS		+=event.h
HEADERS		+=exits.h
HEADERS		+=forwarder.h
HEADERS		+=renderer.h
HEADERS		+=rooms.h
HEADERS		+=stacks.h
HEADERS		+=tree.h
HEADERS		+=userfunc.h
HEADERS		+=utils.h
HEADERS		+=xml2.h

SOURCES		+=auda.cpp
SOURCES		+=croom.cpp
SOURCES		+=configurator.cpp
SOURCES		+=dispatch.cpp
SOURCES		+=engine.cpp
SOURCES		+=event.cpp
SOURCES		+=exits.cpp
SOURCES		+=forwarder.cpp
SOURCES		+=interface.cpp
SOURCES		+=renderer.cpp
SOURCES		+=rooms.cpp
SOURCES		+=stacks.cpp
SOURCES		+=tree.cpp
SOURCES		+=userfunc.cpp
SOURCES		+=utils.cpp
SOURCES		+=xml2.cpp

TARGET		= ../pandora

macx {
	LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
}

win:LIBS	+= -lwsock32
unix:LIBS		+= -lm 
!debug {
	unix:QMAKE_POST_LINK=strip $(TARGET)
}

#CFLAGS_VAR	= $$system(pkg-config --cflags OGRE)
#CLIBS_VAR	= $$system(pkg-config --libs OGRE)
#QMAKE_CXXFLAGS_RELEASE	+=  $$CFLAGS_VAR
#QMAKE_CXXFLAGS_DEBUG	+=  -O2 $$CFLAGS_VAR
#LIBS 		+= $$CLIBS_VAR
