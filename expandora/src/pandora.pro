

TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

CONFIG		+= qt opengl warn_on debug thread

win32 {
	CONFIG	+= console
}

HEADERS		+=config_reader.h
HEADERS		+=configurator.h
HEADERS		+=defines.h
HEADERS		+=dispatch.h
HEADERS		+=engine.h
HEADERS		+=event.h
HEADERS		+=exits.h
HEADERS		+=forwarder.h
HEADERS		+=renderer.h
HEADERS		+=rooms.h
HEADERS		+=stacks.h
HEADERS		+=struct.h
HEADERS		+=tree.h
HEADERS		+=userfunc.h
HEADERS		+=utils.h
HEADERS		+=xml2.h

SOURCES		+=auda.cpp
SOURCES		+=config_reader.cpp
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

unix:LIBS		+= -lm -lqt-mt

!debug {
	unix:QMAKE_POST_LINK=strip $(TARGET)
}

