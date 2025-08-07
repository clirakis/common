##################################################################
#
#	Makefile for xxxx using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	17-Jan-09       CBL     Original
#       02-Apr-22       CBL     Added in RootDataRead and gpxread
#       09-Apr-22       CBL     Added in HDF5 read
#       30-Oct-22       CBL     Geodetic no long necessary since proj8 has
#                               a cpp binding. 
#
######################################################################
# Machine specific stuff
#
#
LIBRARY = $(LIBDIR)/libNav.a 
#
# Compile time resolution.
#
EXT_CFLAGS +=  -g
INCLUDE  = -I/usr/include/hdf5/serial
INCLUDE +=  -I$(DRIVE)/common/utility -I$(ROOT_INC)
LIBS = 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = NavPoint.cpp NavPointList.cpp NavUtility.cpp \
	LSM9DS1.cpp RootDataRead.cpp gpxread.cpp H5read.cpp

SRCS    = $(SRC) $(SRCCPP)

HEADERS = NavPoint.hh NavPointList.hh NavUtility.hh \
	LSM9DS1.hh RootDataRead.hh gpxread.hh H5read.hh

# When we build all, what do we build?
all:      $(LIBRARY)

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
#include make.depend 
# DO NOT DELETE
