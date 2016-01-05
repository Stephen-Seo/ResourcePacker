SRC_ROOT = ../src
CXX = x86_64-w64-mingw32-g++
CXXFLAGS = -c -std=c++11 -Wall -Wextra -O3 -D NDEBUG \
           -I${SRC_ROOT} \
           -mwindows
CXXDEBUGFLAGS = -c -std=c++11 -Wall -Wextra -O0 -g -D_GLIBCXX_DEBUG \
           -I${SRC_ROOT} \
           -mwindows
LINKER_FLAGS = -mwindows -Wall
LINKER_LIBS = 

ResourcePacker_SOURCES = \
    ${SRC_ROOT}/ResourcePacker.o

ResourcePackerBin_SOURCES = \
    ${SRC_ROOT}/main.o

all:    ResourcePacker.dll ResourcePacker.exe

ResourcePacker.dll: ${ResourcePacker_SOURCES}
		$(CXX) $(LINKER_FLAGS) -shared -o ResourcePacker.dll ${LINKER_LIBS} ${ResourcePacker_SOURCES}

ResourcePacker.dll: ${ResourcePacker_SOURCES} ${ResourcePackerBin_SOURCES}
		$(CXX) $(LINKER_FLAGS) -o ResourcePacker.exe ${LINKER_LIBS} ${ResourcePacker_SOURCES} ${ResourcePackerBin_SOURCES}

clean:	../src.clean
		rm -f ResourcePacker.dll
		rm -f ResourcePacker.exe

cleanObjectsOnly:	../src.clean


%.clean:
		rm -f $*/*.o

%.o:		%.cpp
ifeq (${DEBUG},1)
		$(CXX) $(CXXDEBUGFLAGS) -o $*.o $*.cpp
else
		$(CXX) $(CXXFLAGS) -o $*.o $*.cpp
endif
