TEMPLATE	= lib
OBJECTS_DIR	= obj
MOC_DIR		= moc

INCLUDEPATH += . ../include/

CONFIG		+= qt opengl warn_on debug thread

win32 {
	CONFIG	+= console
}
macx {
	HEADERS += cmath
}

HEADERS	+= ../include/Frustum.h
HEADERS += ../include/Component.h
HEADERS += ../include/BaseEvent.h
HEADERS += ../include/Coordinate.h
HEADERS += ../include/ListCycler.h
HEADERS += ../include/ListStack.h
HEADERS += ../include/ObjectRecycler.h
HEARDER += ../include/ParseEvent.h
HEADERS += ../include/Property.h
HEADERS += ../include/Room.h
HEADERS += ../include/RoomCollection.h
HEADERS += ../include/SimpleString.h
HEADERS += ../include/TinyList.h
HEADERS += ../include/AbstractRoomContainer.h
HEADERS += ../include/RoomSignalHandler.h

SOURCES += Frustum.cpp
SOURCES += Component.cpp
SOURCES += BaseEvent.cpp
SOURCES += Coordinate.cpp
SOURCES += ParseEvent.cpp
SOURCES += Property.cpp
SOURCES += Room.cpp
SOURCES += RoomCollection.cpp
SOURCES += SimpleString.cpp
SOURCES += RoomSignalHandler.cpp

DESTDIR = ../lib

!mac:unix:LIBS		+= -lm -lqt-mt
