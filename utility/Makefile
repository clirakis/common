##################################################################
#
#	Makefile for libutility using gcc 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	16-Sep-00       CBL     Original
#       06-Nov-01       CBL     Updated to be more generic for both Linux
#                               and IRIX
#       13-Feb-16       CBL     Changing some things up
#       27-Feb-22       CBL     Split off a file called constants
#       02-Jan-23	CBL	UTC to Seconds
#
######################################################################
# Machine specific stuff
#
#
#
LIBRARY = $(LIBDIR)/libutility.a
#
# Compile time resolution.
#
CFLAGS += -g
INCLUDE = -I/usr/include/hdf5/serial
LIBS    = 
# Rules to make the object files depend on the sources.
SRCCPP  = Point.cpp CLogger.cpp filename.cpp precisetime.cpp \
	AmIRunning.cpp TimeStamp.cpp cvt2jd.cpp CObject.cpp \
	Buffered.cpp tools.cpp H5Logger.cpp Split.cpp UTC2Sec.cpp
SRC     = debug.c 
SRCS    = $(SRC) $(SRCCPP)


HEADERS = debug.h Point.hh CLogger.hh filename.hh precisetime.hh \
	AmIRunning.hh TimeStamp.hh cvt2jd.h CObject.cpp Buffered.hh \
	 tools.hh H5Logger.hh Split.hh Constants.h UTC2Sec.hh

# When we build all, what do we build?
all:   $(LIBRARY)

include ../makefiles/makefile.inc


#dependencies
include make.depend 
