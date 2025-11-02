##################################################################
#
#	Makefile for NMEALIB using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	24-Feb-22       CBL     Original
#       28-Apr-24       CBL     part of common and SO now. 
#       02-Nov-25       CBL     breaking into smaller files. I'll 
#                               eventually add more NMEA messages into
#                               the mix, and want to add writing. 
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
SRCCPP  = NMEA_GPS.cpp GGA.cpp RMC.cpp VTG.cpp GSA.cpp NMEA_Position.cpp \
	NMEA_helper.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = NMEA_GPS.hh GGA.hh RMC.hh VTG.hh GSA.hh NMEA_Position.hh \
	NMEA_helper.hh

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

.PHONY : clean 

clean:
	\rm -f *.d
	\rm -f *.o
	\rm -f *.bak
	\rm -f *~
