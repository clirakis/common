##################################################################
#
#	Makefile for RT_tools shared library using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	26-Feb-24       CBL     Original
#       28-Feb-24	CBL	Added in time utilities
#       17-Mar-24       CBL     Added in queryTimeServer
#
######################################################################
#
# Target
LIBRARY = $(LIBDIR)/libRT_tools.so 
#
# Setup
#
COMMON  = $(DRIVE)/common
#
# Compilier defs.
#
CC  = gcc
CPP = g++
LD  = g++
#
# Compile time resolution.
#
INCLUDE =  -I$(COMMON)/utility -I$(COMMON)/iolib 
CFLAGS = -Wall -Werror -fpic -DPOSIX $(INCLUDE) $(DEFINES) $(EXT_CFLAGS)
LDFLAGS= -shared
#
# Keep around, maybe needed. 
# -fbounds-check

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = SimpleCommand.cpp TimeUtil.cpp Timeout.cpp queryTimeServer.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = SimpleCommand.hh TimeUtil.hh Timeout.hh queryTimeServer.hh

.cpp.o:
	$(CPP) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

# Rules to make the object files depend on the sources.
OBJSC   = ${SRC:.c=.o}
OBJSCPP = ${SRCCPP:.cpp=.o}
OBJS    = $(OBJSC) $(OBJSCPP)


$(LIBRARY): $(OBJS) $(HEADERS)
	$(LD) $(LDFLAGS) -o $(LIBRARY) $(OBJS)

# When we build all, what do we build?
all:      $(LIBRARY) make.depend

.PHONY : clean make.depend

clean:
	\rm -f *.d
	\rm -f *.o
	\rm -f make.depend
	\rm -f *.bak
	\rm -f *~
	\rm -f *Dict*
	\rm -f $(TARGET)
