#OBJS = files to compile for project
DEFAULT = projectiles

DEFAULT_OBJS = ../LTexture.cpp ../LPlayer.cpp ../LTimer.cpp ../LCircle.cpp \
			   ./JGame.cpp ./LProjectile.cpp ./LBullet.cpp ./LEntity.cpp

#CC = the compiler
CC = g++

#INCLUDE_PATHS = the include paths
INCLUDE_PATHS = -IC:\mingw\include\SDL2 
#LIBRARY_PATHS = the library paths
LIBRARY_PATHS = -LC:\mingw\lib

#COMPILER_FLAGS = compile options
# -w suppress warning
# -Wl,-subsystem, windows gets rid of console window
COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS = libraries to link
LINKER_FLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf \
			   -lSDL2_mixer

#target to compile
default: $(DEFAULT).cpp
	$(CC) $(DEFAULT).cpp $(DEFAULT_OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) \
		$(LINKER_FLAGS) -o $(DEFAULT)
