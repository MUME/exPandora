CONFIG += qt thread warn_off exceptions link_pkgconfig compile_pkgconfig 
QT += xml gui
#network
PKGCONFIG += OGRE 

CONFIG += debug

win32 {
	CONFIG	+= console
}

QMAKE_CXXFLAGS_RELEASE += -IOgre -O0 -g
QMAKE_CXXFLAGS_DEBUG += -IOgre -O0 -g




TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

FORMS +=	configedit.ui
FORMS +=        roomedit.ui
FORMS +=	spellsdialog.ui

HEADERS		+=configurator.h
HEADERS		+=CRoom.h
HEADERS		+=Regions.h
HEADERS		+=defines.h
HEADERS		+=dispatch.h
HEADERS		+=engine.h
HEADERS		+=exits.h
HEADERS		+=forwarder.h
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
HEADERS 	+=QOgreWidget.h
HEADERS 	+=ExampleApplication.h
HEADERS		+=SceneManager.h

SOURCES		+=auda.cpp
SOURCES		+=CRoom.cpp
SOURCES		+=Regions.cpp
SOURCES		+=configurator.cpp
SOURCES		+=dispatch.cpp
SOURCES		+=engine.cpp
SOURCES		+=exits.cpp
SOURCES		+=forwarder.cpp
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
SOURCES 	+=QOgreWidget.cpp
SOURCES 	+=ExampleApplication.cpp
SOURCES		+=SceneManager.cpp


TARGET		= ../pandora

macx {
	LIBS += /System/Library/Frameworks/CoreFoundation.framework/Versions/A/CoreFoundation
}

win32:LIBS	+= -lwsock32
unix:LIBS		+= -lm
!debug {
	unix:QMAKE_POST_LINK=strip $(TARGET)
}

