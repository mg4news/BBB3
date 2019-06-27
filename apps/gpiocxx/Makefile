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
gpiocxx_cpp := $(shell pwd)/src/main.cpp

# posutils (C source)
posutils_dir := $(root_dir)/libs/posutils
posutils_c := $(posutils_dir)/posutils.c \
	$(posutils_dir)/pumutex.c \
	$(posutils_dir)/puthread.c
	
#------------------------------------------------------------------------------
# Includes for the LIBGPIOD library
# Accounts for both "gpiod.h" and "gpiod.hpp"
#------------------------------------------------------------------------------
GPIOD_INC := -I$(root_dir)/libgpiod/include -I$(root_dir)/libgpiod/bindings/cxx
	
#------------------------------------------------------------------------------
# Executable, C source list, CPP source list
##SYS_INC  := $(shell pkg-config --cflags $(LIB_LST))
# Haven't quite figured out pkg-config and cross compiling, so the header files are physically copied into
# a special sysinc directory
#------------------------------------------------------------------------------
LOCAL_INC := $(GPIOD_INC) -I$(root_dir)/include
SYS_INC :=
EXECUTABLE:= gpiocxx
C_SRC   := $(posutils_c)
CPP_SRC := $(gpiocxx_cpp) 

#------------------------------------------------------------------------------
# Library lists, for dynamically linked libraries. Should normally only be "glib"
# Note: "lib_lst" is resolved using pkg-config
# Note: "extra-libs" are passed directly to the compiler as options 
#------------------------------------------------------------------------------
# LIB_LST := glib-2.0
# Then generate links with := $(shell pkg-config --libs $(LIB_LST))
# BUT..I havent figured this one out, so:
# - first I run pkg-config --lib on the BBB3 board, and use that in the makefile
# For the include files I add them to a local sysinc directory
LIB_GPIOD := -L/usr/local/lib -lgpiod
LIB_LST := $(LIB_GPIOD)
EXTRA_LIBS := -lpthread -lrt -pthread

#------------------------------------------------------------------------------
# Definitions in the form -Dxxxxx
#------------------------------------------------------------------------------
DEFINED := 

###############################################################################
# DONT MODIFY ANYTHING ELSE BELOW THIS LINE
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
gcc_dir := /workspace/gcc-bbb3/bin
CC      := $(gcc_dir)/arm-linux-gnueabihf-gcc
CPP     := $(gcc_dir)/arm-linux-gnueabihf-g++
STRIP   := $(gcc_dir)/arm-linux-gnueabihf-strip

#------------------------------------------------------------------------------
# Compile settings
#------------------------------------------------------------------------------
CFLAGS  := $(BUILD_OPTIONS) $(SYS_INC) $(LOCAL_INC) $(DEFINED) $(C_ONLY_DEFS)
CPPFLAGS:= -std=c++1y $(BUILD_OPTIONS) $(SYS_INC) $(LOCAL_INC) $(DEFINED)
##LDFLAGS := $(shell pkg-config --libs $(LIB_LST)) $(EXTRA_LIBS)
LDFLAGS :=  $(LIB_LST) $(EXTRA_LIBS)

C_OBJS    := $(patsubst %.c, %.o, $(C_SRC))
CPP_OBJS  := $(patsubst %.cpp, %.o, $(CPP_SRC))

strip: clean $(EXECUTABLE)
	$(STRIP) --strip-unneeded $(EXECUTABLE) 

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



	


