##################################################################
#
#	Makefile for libKeithley using gcc on Linux. 
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
LIBRARY = $(LIBDIR)/libKeithley.a
#
# Compile time resolution.
#
INCLUDE =  -I$(DRIVE)/common/GPIB -I$(DRIVE)/common/utility 
LIBS = 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = Keithley196.cpp  Keithley197.cpp  Keithley2x0.cpp  Keithley.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = Keithley196.hh  Keithley197.hh  Keithley2x0.hh  Keithley.hh

# When we build all, what do we build?
all: $(LIBRARY)      

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
#include make.depend 
# DO NOT DELETE
