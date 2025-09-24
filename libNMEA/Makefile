##################################################################
#
#	Makefile for NMEALIB using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	24-Feb-22       CBL     Original
#       28-Apr-24       CBL     part of common and SO now. 
#
######################################################################
# Machine specific stuff
#
#
# Target
LIBRARY = $(LIBDIR)/libNMEA.so 
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
INCLUDE = -I$(DRIVE)/common/utility 
CFLAGS = -Wall -Werror -fpic -DPOSIX $(INCLUDE) $(DEFINES) $(EXT_CFLAGS)
LDFLAGS= -shared

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = NMEA_GPS.cpp 
SRCS    = $(SRC) $(SRCCPP)

HEADERS = NMEA_GPS.hh

# When we build all, what do we build?
all:      $(LIBRARY)

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
all:      $(LIBRARY) 

.PHONY : clean make.depend

clean:
	\rm -f *.d
	\rm -f *.o
	\rm -f make.depend
	\rm -f *.bak
	\rm -f *~
	\rm -f *Dict*
	\rm -f $(TARGET)
