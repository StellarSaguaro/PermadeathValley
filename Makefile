#SRCS specifies which files to compile as part of the project
SRCS  = main.cc 
SRCS += rogrand.hh rogrand.cc
SRCS += gamemaster.hh gamemaster.cc
SRCS += gameboard.hh gameboard.cc
SRCS += tile.hh tile.cc
#SRCS += river.hh river.cc
SRCS += pawn.hh pawn.cc
SRCS += navigator.hh navigator.cc
SRCS += FastNoiseLite.h

#CC specifies which compiler
CC = g++ -std=c++11

#CC_FLAGS specifies the additional compilation options
CC_FLAGS  = -Wall   # All Warnings
CC_FLAGS += -g      # Debug Info

# Requires that libsdl2-dev and libsdl2-image-* be installed
CC_FLAGS += -I/usr/include/SDL2

#LINK_FLAGS specifies the libraries to linking against
LINK_FLAGS =
ifeq ($(OS),Windows_NT)
LINK_FLAGS += -L./SDL2_WIN/lib -lmingw32 -lSDL2main
endif
LINK_FLAGS += -lSDL2 -lSDL2_image

#OUT_NAME specifies the name of the exectuable
ifeq ($(OS),Windows_NT)
OUT_NAME = permadeathvalley_WINDOWS
else
OUT_NAME = permadeathvalley.linux
endif

#This is the target that compiles the executable
all : $(SRCS)
	$(CC) $(SRCS) $(CC_FLAGS) $(LINK_FLAGS) -o $(OUT_NAME)
