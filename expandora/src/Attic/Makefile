CXXFLAGS=-g -I/Users/alve/Library/qt/include -I. -I/usr/include/libxml2
;CXXFLAGS=-O3 -pipe -maltivec -mabi=altivec -mcpu=7450 -mpowerpc-gfxopt -I/Users/alve/Library/qt/include -I. -I/usr/include/libxml2
LDFLAGS=-lm -lqt-mt -L/Users/alve/Library/qt/lib -L/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/ -lGLU -lGL -lxml2
OBS=Makefile tinyList.o searchTreeNode.o characterTable.o auda.o engine.o forwarder.o rooms.o userfunc.o config_reader.o event.o interface.o stacks.o utils.o dispatch.o exits.o renderer.o tree.o xml2.o moc_renderer.o

all: ${OBS}
	g++ ${CXXFLAGS} ${LDFLAGS} -o ../pandora *.o

moc_renderer.cpp: Makefile
	moc renderer.h -o moc_renderer.cpp


%.o: %.cpp %.h Makefile
	g++ ${CXXFLAGS} -c $< -o $@

clean:
	rm -f *.o
	rm -f moc_renderer.cpp
	rm -f ../pandora


