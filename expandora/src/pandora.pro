
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

LEXSOURCES	+=mud.lex
LEXSOURCES	+=player.lex


HEADERS         +=ListCycler.h
HEADERS         +=SimpleString.h
HEADERS		+=BaseEvent.h
HEADERS		+=CharacterTable.h
HEADERS		+=Coordinate.h
HEADERS		+=IntermediateNode.h
HEADERS		+=LexDefs.h
HEADERS		+=Lexer.h
HEADERS		+=Map.h
HEADERS		+=ObjectRecycler.h
HEADERS		+=ParseEvent.h
HEADERS		+=Parser.h
HEADERS		+=Path.h
HEADERS		+=Property.h
HEADERS		+=Room.h
HEADERS		+=RoomAdmin.h
HEADERS		+=RoomCollection.h
HEADERS		+=RoomSearchNode.h
HEADERS		+=SearchTreeNode.h
HEADERS		+=Terrain.h
HEADERS		+=TinyList.h
HEADERS		+=config_reader.h
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
HEADERS         +=TelnetFilter.h

SOURCES         +=SimpleString.cpp
SOURCES         +=TelnetFilter.cpp
SOURCES		+=BaseEvent.cpp
SOURCES		+=Terrain.cpp
SOURCES		+=CharacterTable.cpp
SOURCES		+=Coordinate.cpp
SOURCES		+=IntermediateNode.cpp
SOURCES		+=Lexer.cpp
SOURCES		+=Map.cpp
SOURCES		+=ParseEvent.cpp
SOURCES		+=Parser.cpp
SOURCES		+=Path.cpp
SOURCES		+=Property.cpp
SOURCES		+=Room.cpp
SOURCES		+=RoomAdmin.cpp
SOURCES		+=RoomCollection.cpp
SOURCES		+=SearchTreeNode.cpp
SOURCES		+=auda.cpp
SOURCES		+=config_reader.cpp
SOURCES		+=dispatch.cpp
SOURCES		+=engine.cpp
SOURCES		+=event.cpp
SOURCES		+=exits.cpp
SOURCES		+=forwarder.cpp
SOURCES		+=interface.cpp
SOURCES		+=qtxml.cpp
SOURCES		+=renderer.cpp
SOURCES		+=rooms.cpp
SOURCES		+=stacks.cpp
SOURCES		+=tree.cpp
SOURCES		+=userfunc.cpp
SOURCES		+=utils.cpp

TARGET		= ../pandora

!mac:unix:LIBS		+= -lm -lqt-mt



