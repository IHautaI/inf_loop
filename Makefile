VPATH = lib/
CXX = clang++
CPPFLAGS = -O3 --std=c++11 -Wall
OBJS = Tile.o Grid.o inf_loop.o
LDLIBS = -lboost_program_options -lpthread -lstdc++

inf_loop : $(OBJS)

inf_loop.o : Grid.o Grid.hpp

Grid.o : Grid.cpp Grid.hpp

Tile.o : Tile.cpp Tile.hpp

clean : 
	rm inf_loop $(OBJS)
