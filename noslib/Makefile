##################################################################
#
#	Makefile for libnos using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	26-May-09       CBL     Original
#       01-Oct-22       CBL     move to common
#
######################################################################
# Machine specific stuff
#
#
LIBRARY = $(HOME)/lib_Linux/libNOS.a 
#
# Compile time resolution.
#
INCLUDE =  -I$(DRIVE)/common/utility
LIBS = 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = nos_header.cpp geo_header.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = nos_header.hh geo_header.hh

# When we build all, what do we build?
all:      $(LIBRARY)

include $(DRIVE)/common/makefiles/makefile.inc

#dependencies
include make.depend 
# DO NOT DELETE
