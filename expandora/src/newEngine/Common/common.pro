TEMPLATE	= app
OBJECTS_DIR	= obj
MOC_DIR		= moc

CONFIG		+= qt opengl warn_on debug thread

win32 {
	CONFIG	+= console
}
macx {
	HEADERS += cmath
}


HEADERS += BaseEvent.h
HEADERS += Coordinate.h
HEADERS += ListCycler.h
HEADERS += ListStack.h
HEADERS += ObjectRecycler.h
HEARDER += ParseEvent.h
HEADERS += Property.h
HEADERS += Room.h
HEADERS += RoomCollection.h
HEADERS += SimpleString.h
HEADERS += TinyList.h
HEADERS += AbstractRoomContainer.h


SOURCES += BaseEvent.cpp
SOURCES += Controller.cpp
SOURCES += Coordinate.cpp
SOURCES += ParseEvent.cpp
SOURCES += Property.cpp
SOURCES += Room.cpp
SOURCES += RoomCollection.cpp
SOURCES += SimpleString.cpp

TARGET		= ../expandora

!mac:unix:LIBS		+= -lm -lqt-mt
