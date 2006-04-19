

TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

CONFIG		+= qt opengl warn_on thread release

QT += xml opengl gui network

win32 {
	CONFIG	+= console
}

FORMS +=	configedit.ui
FORMS +=        roomedit.ui
FORMS +=	spellsdialog.ui

HEADERS		+=configurator.h
HEADERS		+=CRoom.h
HEADERS		+=defines.h
HEADERS		+=dispatch.h
HEADERS		+=engine.h
HEADERS		+=exits.h
HEADERS		+=forwarder.h
HEADERS		+=renderer.h
HEADERS		+=Map.h
HEADERS		+=stacks.h
HEADERS		+=tree.h
HEADERS		+=userfunc.h
HEADERS		+=utils.h
HEADERS		+=xml2.h
HEADERS		+=RoomEditDialog.h
HEADERS		+=ConfigWidget.h
HEADERS		+=SpellsDialog.h
HEADERS		+=mainwindow.h


SOURCES		+=auda.cpp
SOURCES		+=CRoom.cpp
SOURCES		+=configurator.cpp
SOURCES		+=dispatch.cpp
SOURCES		+=engine.cpp
SOURCES		+=exits.cpp
SOURCES		+=forwarder.cpp
SOURCES		+=renderer.cpp
SOURCES		+=Map.cpp
SOURCES		+=stacks.cpp
SOURCES		+=tree.cpp
SOURCES		+=userfunc.cpp
SOURCES		+=utils.cpp
SOURCES		+=xml2.cpp
SOURCES		+=mainwindow.cpp
SOURCES		+=RoomEditDialog.cpp
SOURCES		+=ConfigWidget.cpp
SOURCES		+=SpellsDialog.cpp


TARGET		= ../pandora

macx {
	LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
}

win32:LIBS	+= -lwsock32
unix:LIBS		+= -lm 
!debug {
	unix:QMAKE_POST_LINK=strip $(TARGET)
}

#CFLAGS_VAR	= $$system(pkg-config --cflags OGRE)
#CLIBS_VAR	= $$system(pkg-config --libs OGRE)
#QMAKE_CXXFLAGS_RELEASE	+=  $$CFLAGS_VAR
#QMAKE_CXXFLAGS_DEBUG	+=  -O2 $$CFLAGS_VAR
#LIBS 		+= $$CLIBS_VAR
