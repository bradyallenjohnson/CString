SHELL=/bin/bash
CC=/usr/bin/g++
DELETE_CMD=/bin/rm -f

LIBRARY_PATH=-L..
INCLUDE_PATH=-I..
LIBS=-lCString

CCFLAGS=-g
OBJ_EXTENSION=.o

EXE_NAME=TestCString
OBJS=TestCString$(OBJ_EXTENSION)

#
# Targets
#

all: $(EXE_NAME) $(OBJS)

$(EXE_NAME): $(OBJS)
	$(CC) $(CCFLAGS) $(LIBRARY_PATH) -o $(EXE_NAME) $(OBJS) $(LIBS)

TestCString$(OBJ_EXTENSION): TestCString.cpp
	$(CC) $(CCFLAGS) $(INCLUDE_PATH) -c TestCString.cpp -o TestCString$(OBJ_EXTENSION)

clean:
	$(DELETE_CMD) $(OBJS) $(EXE_NAME)
