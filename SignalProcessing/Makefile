##################################################################
#
#	Makefile for libSignal using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	21-Dec-23       CBL     Original
#       26-Dec-23	CBL 	Changed to shared library format
#	30-Dec-23	CBL	Added in a very simple low pass filter class
#
######################################################################
###
# Target
#
#
LIBRARY = $(HOME)/lib_Linux/libSignal.so
# Machine specific stuff
#
# Compilier defs.
#
CC  = gcc
CPP = g++
LD  = g++
#
# Compile time resolution.
#
INCLUDE =  -I$(DRIVE)/common/utility
LIBS = 
#
# Other useful defines.
# 
COMMON=$(DRIVE)/common
CFLAGS = -Wall -Werror -fpic -DPOSIX $(INCLUDE) $(DEFINES) $(EXT_CFLAGS)
LDFLAGS= -shared
# Rules to make the object files depend on the sources.
SRCS    = $(SRC) $(SRCCPP)

# Rules to make the object files depend on the sources.
OBJSC   = ${SRC:.c=.o}
OBJSCPP = ${SRCCPP:.cpp=.o}
OBJS    = $(OBJSC) $(OBJSCPP)


# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = Butterworth.cpp Filt.cpp SFilter.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = Butterworth.hh Filt.hh SFilter.hh

.cpp.o:
	$(CPP) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

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


#dependencies
#include make.depend 
# DO NOT DELETE
