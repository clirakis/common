##################################################################
#
#	Makefile for libmygpib using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	27-Nov-14       CBL     Original
#
######################################################################
# Machine specific stuff
#
#

LIBRARY = $(LIBDIR)/libmygpib.a
#
# Compile time resolution.
#
INCLUDE =  -I$(DRIVE)/common/utility -I$(ROOT_INC)
LIBS = 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = GPIB.cpp 
SRCS    = $(SRC) $(SRCCPP)

HEADERS = GPIB.cpp 

# When we build all, what do we build?
all: $(LIBRARY)      

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
#include make.depend 
# DO NOT DELETE
