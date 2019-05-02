#==============================================================================
# Copyright (c) Martin Gibson
# Simple platform independent makefile 
# The "pkg-config" utility is used to resolve the library names, paths and linkage
#==============================================================================
root_dir:= $(shell pwd)/../..

###############################################################################
# CAN MODIFY THE NEXT 4 SECTIONS
# - LOCAL INCLUDES (leave empty if not used)
# - LISTS of C SOURCE (leave empty if not used)
# - LISTS OF C++ SOURCE (leave empty if not used)
# - EXECUTABLE, C_SRC, CPP_SRC
# - SYSTEM LIBRARIES
###############################################################################

#------------------------------------------------------------------------------
# Include paths, and source lists
#------------------------------------------------------------------------------
simple_cpp := $(shell pwd)/src/simple.cpp

#------------------------------------------------------------------------------
# Executable, C source list, CPP source list
#------------------------------------------------------------------------------
LOCAL_INC := -I$(root_dir)/include
EXECUTABLE:= simple
C_SRC   := 
CPP_SRC := $(simple_cpp) 

#------------------------------------------------------------------------------
# Library lists, for dynamically linked libraries. Should normally only be "glib"
# Note: "lib_lst" is resolved using pkg-config
# Note: "extra-libs" are passed directly to the compiler as options 
#------------------------------------------------------------------------------
#LIB_LST := glib-2.0
LIB_LST := 
EXTRA_LIBS := -lpthread -lrt -pthread

#------------------------------------------------------------------------------
# Definitions in the form -Dxxxxx
#------------------------------------------------------------------------------
DEFINED := 

###############################################################################
# DONT MODIFY ANYTHINF ELSE BELOW THIS LINE
###############################################################################

#------------------------------------------------------------------------------
# ERRORS AND WARNINGS
# These are strict, its WAY better to catch issues at build time than at run time
#------------------------------------------------------------------------------
BUILD_ERR  := -Werror=shadow -Werror=undef -Werror=uninitialized -Werror=implicit -Werror=missing-prototypes -Werror=cast-align 
ERROR_64BIT := -Werror=pointer-to-int-cast -Werror=int-to-pointer-cast -Werror=conversion -Werror=sign-conversion
BUILD_WARN := -Wall -Wunreachable-code -Wparentheses -Wswitch -Wunused-function -Wformat
BUILD_OPTIONS := -g $(BUILD_WARN) $(BUILD_ERR) $(ERROR_64BIT)

#------------------------------------------------------------------------------
# Cross compiler
#------------------------------------------------------------------------------
gcc_dir := /workspace/gcc-bbb3
CC  := $(gcc_dir)/bin/arm-linux-gnueabihf-gcc
CPP := $(gcc_dir)/bin/arm-linux-gnueabihf-g++

#------------------------------------------------------------------------------
# Compile settings
#------------------------------------------------------------------------------
##SYS_INC  := $(shell pkg-config --cflags $(LIB_LST))
SYS_INC := 
CFLAGS  := $(BUILD_OPTIONS) $(SYS_INC) $(LOCAL_INC) $(DEFINED) $(C_ONLY_DEFS)
CPPFLAGS:= -std=c++1y $(BUILD_OPTIONS) $(SYS_INC) $(LOCAL_INC) $(DEFINED)
##LDFLAGS := $(shell pkg-config --libs $(LIB_LST)) $(EXTRA_LIBS)
LDFLAGS := $(EXTRA_LIBS)

C_OBJS    := $(patsubst %.c, %.o, $(C_SRC))
CPP_OBJS  := $(patsubst %.cpp, %.o, $(CPP_SRC))

#strip: clean $(EXECUTABLE)
#	@strip --strip-unneeded $(EXECUTABLE) 

all: clean $(EXECUTABLE)

clean: 
	$(RM) $(EXECUTABLE)
	$(RM) $(C_OBJS)
	$(RM) $(CPP_OBJS)

$(EXECUTABLE): $(C_OBJS) $(CPP_OBJS)
	$(CPP) -o $@ $(C_OBJS) $(CPP_OBJS) $(LDFLAGS)

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@
	
%.o : %.cpp
	$(CPP) -c $(CPPFLAGS) $< -o $@



	


