##################################################################
#
#	Makefile for libNavBasic using gcc on Linux. 
#       navigation library that only used straight c++
#
#
#	Modified	by	Reason
# 	--------	--	------
#	20-Feb-17       CBL     Original
#       27-Feb-22       CBL     moved to common
#	30-Oct-22	CBL     Geodetic is no longer necessary. 
#                               system upgraded to Proj8. This is C++
#                               based. 
#
######################################################################
# Machine specific stuff
#
#
LIBRARY = $(LIBDIR)/libNavBasic.a 
#
# Compile time resolution.
#
INCLUDE =  -I$(DRIVE)/common/utility 
LIBS = 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = NavPoint.cpp NavPointList.cpp NavUtility.cpp

SRCS    = $(SRC) $(SRCCPP)

HEADERS = NavPoint.hh NavPointList.hh NavUtility.hh 

# When we build all, what do we build?
all:      $(LIBRARY)

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
#include make.depend 
# DO NOT DELETE
