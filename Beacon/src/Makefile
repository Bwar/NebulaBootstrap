CC = gcc
CXX = g++
CFLAGS = -g -O2 -fPIC 
CXXFLAG =  -std=c++14 -O2 -Wall -ggdb -m64 -D_GNU_SOURCE=1 -D_REENTRANT -D__GUNC__ -fPIC -DNODE_BEAT=10.0 

ARCH:=$(shell uname -m)

ARCH32:=i686
ARCH64:=x86_64

ifeq ($(ARCH),$(ARCH64))
SYSTEM_LIB_PATH:=/usr/lib64
else
SYSTEM_LIB_PATH:=/usr/lib
endif
LIB3RD_PATH = ../../../../factory/cplus/NebulaDepend
NEBULA_PATH = ../../../../factory/cplus/Nebula
BEACON_PATH = ..

VPATH = $(BEACON_PATH)/src
SUB_DIRS := $(foreach dir, $(VPATH), $(shell find $(dir) -maxdepth 5 -type d))
DIRS := $(SUB_DIRS)


INC := $(INC) \
       -I $(LIB3RD_PATH)/include \
       -I $(NEBULA_PATH)/include \
       -I $(BEACON_PATH)/src 


LDFLAGS := $(LDFLAGS) -D_LINUX_OS_ \
           -L$(NEBULA_PATH)/lib -lnebula \
           -L$(LIB3RD_PATH)/lib -lhiredis \
           -L$(LIB3RD_PATH)/lib -lcryptopp \
           -L$(LIB3RD_PATH)/lib -lprotobuf \
           -L$(LIB3RD_PATH)/lib -lev \
           -L$(SYSTEM_LIB_PATH) -lc -lrt -ldl

CPP_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
CC_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
C_SRCS = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
OBJS = $(patsubst %.cpp,%.o,$(CPP_SRCS)) $(patsubst %.c,%.o,$(C_SRCS)) $(patsubst %.cc,%.o,$(CC_SRCS))

TARGET = Beacon

all: $(TARGET)
	
Beacon: $(OBJS)
	$(CXX) $(CXXFLAG) $(DEFINE_SERVER) -o $@ $(OBJS) $(LDFLAGS) $(INC)
	mkdir -p $(BEACON_PATH)/bin	
	cp -f $@ $(BEACON_PATH)/bin/

%.o:%.cpp
	$(CXX) $(INC) $(CXXFLAG) $(CFLAGS) $(DEFINE_SERVER) -c -o $@ $< $(LDFLAGS)
%.o:%.cc
	$(CXX) $(INC) $(CXXFLAG) -c -o $@ $< $(LDFLAGS)
%.o:%.c
	$(CC) $(INC) $(CFLAGS) -c -o $@ $< $(LDFLAGS)
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)
	rm -rf $(BEACON_PATH)/bin
