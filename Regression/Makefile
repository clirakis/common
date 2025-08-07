##################################################################
#
#	Makefile for regression using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	27-Feb-24       CBL     Original
#       07-Mar-24       CBL     Added in libjsoncpp
#
#
######################################################################
# Machine specific stuff
#
#
TARGET = regression
#
# Compile time resolution.
#
INCLUDE = -I$(DRIVE)/common/StatPak -I$(DRIVE)/common/RT_Tools \
	-I$(DRIVE)/common/iolib \
	-I$(DRIVE)/common/utility
LIBS = -lStatPak -lRT_tools -lio  -lutility -ljsoncpp

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = main.cpp UserSignals.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = UserSignals.hh

# When we build all, what do we build?
all:      $(TARGET)

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
#include make.depend 
# DO NOT DELETE
