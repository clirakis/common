##################################################################
#
#	Makefile for xxxx using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	15-Dec-02       CBL     Original
#
######################################################################
# Machine specific stuff
#
#
LIBRARY = $(LIBDIR)/libTSIP.a 
DOCUMENTATION = /home/clirakis/common/doxygen/index.html
COMMON=$(DRIVE)/common
#
# Compile time resolution.
#
INCLUDE =  -I$(COMMON)/utility -I$(COMMON)/iolib 
LIBS = 

EXT_CFLAGS = -g
# -fbounds-check

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = lassen.cpp  GPSDataPacket.cpp SolutionStatus.cpp RawTracking.cpp \
	TSIPUtility.cpp TSIPosition.cpp TSIPVelocity.cpp GPSTime.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = lassen.hh GPSDataPacket.hh SolutionStatus.hh RawTracking.hh \
	TSIP_Constants.hh TSIPUtilty.hh TSIPposition.hh TSIPVelocity.hh \
	GPSTime.hh

#DOXYGEN: $(HEADERS) 
#	doxygen LassenLib.dox

# When we build all, what do we build?
all:      $(LIBRARY)

include $(DRIVE)/common/makefiles/makefile.inc

#dependencies
include make.depend 
# DO NOT DELETE
