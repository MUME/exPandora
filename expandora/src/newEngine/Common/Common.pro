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
           ../include/RoomCollection.h \
           ../include/SimpleString.h \
           ../include/TinyList.h \
           ../include/AbstractRoomContainer.h \
           ../include/RoomSignalHandler.h \ 
	../include/ParseEvent.h 
SOURCES += Frustum.cpp \
           Component.cpp \
           BaseEvent.cpp \
           Coordinate.cpp \
           ParseEvent.cpp \
           Property.cpp \
           Room.cpp \
           RoomCollection.cpp \
           SimpleString.cpp \
           RoomSignalHandler.cpp
TEMPLATE = lib
OBJECTS_DIR = obj
MOC_DIR = moc
INCLUDEPATH += ../include
CONFIG += debug thread qt opengl warn_on
DESTDIR = ../lib
win32{
  CONFIG += console
}

