# Diese Datei wurde mit dem qmake-Manager von KDevelop erstellt. 
# ------------------------------------------- 
# Unterverzeichnis relativ zum Projektverzeichnis: ./src/newEngine/Common
# Das Target ist eine Bibliothek:  

HEADERS += ../include/Frustum.h \
           ../include/Component.h \
           ../include/BaseEvent.h \
           ../include/Coordinate.h \
           ../include/ListCycler.h \
           ../include/ListStack.h \
           ../include/ObjectRecycler.h \
           ../include/Property.h \
           ../include/Room.h \
           ../include/SimpleString.h \
           ../include/TinyList.h \
	../include/ParseEvent.h \
	../include/RoomRecipient.h \
	../include/RoomAdmin.h
SOURCES += Frustum.cpp \
           Component.cpp \
           BaseEvent.cpp \
           Coordinate.cpp \
           ParseEvent.cpp \
           Property.cpp \
           Room.cpp \
           SimpleString.cpp 
TEMPLATE = lib
OBJECTS_DIR = obj
MOC_DIR = moc
INCLUDEPATH += ../include
CONFIG += debug thread qt opengl warn_on
DESTDIR = ../lib
win32{
  CONFIG += console
}

