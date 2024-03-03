##################################################################
#
#	Makefile for StatPak shared library using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	27-Feb-24       CBL     Original
#
######################################################################
#
# Target
LIBRARY = $(LIBDIR)/libStatPak.so 
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
SRCCPP  = Average.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = Average.hh

.cpp.o:
	$(CPP) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

# Rules to make the object files depend on the sources.
OBJSC   = ${SRC:.c=.o}
OBJSCPP = ${SRCCPP:.cpp=.o}
OBJS    = $(OBJSC) $(OBJSCPP)


$(LIBRARY): $(OBJS)
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
