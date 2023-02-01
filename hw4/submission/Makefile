CC = gcc
CFLAGS = -g -Wall -Wextra -pedantic -std=gnu99 -fPIC
CXX = g++
CONSERVATIVE_FLAGS = -std=c++14 -Wall -Wextra -pedantic -fPIC
DEBUGGING_FLAGS = -g -O0
CXXFLAGS = $(CONSERVATIVE_FLAGS) $(DEBUGGING_FLAGS)

PLUGIN_SRCS = swapbg.c mirrorh.cpp mirrorv.cpp tile.cpp expose.cpp
SRCS = imgproc.c pnglite.c $(PLUGIN_SRCS)

OBJS = $(SRCS:%.c=%.o)
PLUGINS = $(PLUGIN_SRCS:%.cpp=plugins/%.so) $(PLUGIN_SRCS:%.c=plugins/%.so)##do i need .c for swapbg?

%.o : %.c
	$(CC) $(CFLAGS) -c $*.c -o $*.o

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $*.cpp -o $*.o

# TODO: use $(CXX) rather than $(CC)
plugins/%.so : %.o
	mkdir -p plugins
	$(CXX) -o plugins/$*.so -shared $*.o

all : imgproc $(PLUGINS)

imgproc : imgproc.o image.o pnglite.o
	$(CXX) -export-dynamic -o $@ imgproc.o image.o pnglite.o -lz -ldl



#imgproc.o : imgproc.cpp imgproc.h
	#$(CC) $(CFLAGS) -c imgproc.cpp

#image.o : image.c image.h
	#$(CC) $(CFLAGS) -c image.c

#pnglite.o : pnglite.c pnglite.h
	#$(CC) $(CFLAGS) -c pnglite.c

plugins/swapbg.so : swapbg.o

.PHONY: solution.zip
solution.zip :
	rm -f solution.zip
	zip -9r $@ Makefile *.h *.cpp *.md *.c

clean :
	rm -f *.o imgproc plugins/*.so depend.mak solution.zip

# Running
#    make depend
# will automatically generate header file dependencies.
depend :
	$(CC) $(CFLAGS) -M $(SRCS) >> depend.mak

depend.mak :
	touch $@

include depend.mak
