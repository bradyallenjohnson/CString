SHELL=/bin/bash
CC=/usr/bin/g++
DELETE_CMD=/bin/rm -f

LIBRARY_PATH=
INCLUDE_PATH=-I.

CCFLAGS=-g -fPIC
LDFLAGS=-shared
OBJ_EXTENSION=.o

LIB_NAME=libCString.so
OBJS=CString$(OBJ_EXTENSION)

#
# Targets
#

all: $(LIB_NAME) $(OBJS)

$(LIB_NAME): $(OBJS)
	$(CC) $(CCFLAGS) $(LDFLAGS) $(LIBRARY_PATH) -o $(LIB_NAME) $(OBJS)

CString$(OBJ_EXTENSION): CString.cpp
	$(CC) $(CCFLAGS) $(INCLUDE_PATH) -c CString.cpp -o CString$(OBJ_EXTENSION)

clean:
	$(DELETE_CMD) $(OBJS) $(LIB_NAME)
