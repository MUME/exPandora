
TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

CFLAGS_VAR	= $$system(pkg-config --cflags libxml-2.0)
CLIBS_VAR	= $$system(pkg-config --libs libxml-2.0)
QMAKE_CXXFLAGS_RELEASE	+=  $$CFLAGS_VAR
QMAKE_CXXFLAGS_DEBUG	+=  $$CFLAGS_VAR
LIBS 		+= $$CLIBS_VAR

CONFIG		+= console qt opengl warn_on release debug

#LIBS 		+= libxml2.lib

HEADERS		+= config_reader.h
HEADERS		+= CharacterTable.h
HEADERS		+= defines.h
HEADERS		+= dispatch.h
HEADERS		+= engine.h
HEADERS		+= event.h
HEADERS		+= exits.h
HEADERS		+= forwarder.h
HEADERS		+= renderer.h
HEADERS		+= rooms.h 
HEADERS		+= stacks.h
HEADERS		+= struct.h 
HEADERS		+= tree.h 
HEADERS		+= userfunc.h
HEADERS		+= utils.h
HEADERS		+= xml2.h 

SOURCES		+= auda.cpp 
SOURCES		+= CharacterTable.cpp
SOURCES		+= config_reader.cpp 
SOURCES		+= dispatch.cpp 
SOURCES		+= engine.cpp 
SOURCES		+= event.cpp 
SOURCES		+= exits.cpp
SOURCES		+= forwarder.cpp
SOURCES		+= renderer.cpp
SOURCES		+= rooms.cpp
SOURCES		+= stacks.cpp
SOURCES		+= tree.cpp
SOURCES		+= userfunc.cpp
SOURCES		+= utils.cpp
SOURCES		+= qtxml.cpp
SOURCES		+= interface.cpp

TARGET		= ../pandora

!mac:unix:LIBS		+= -lm -lqt-mt



