##################################################################
#
#	Makefile for HP code using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	23-Jan-21       CBL     Support for HP3314A function generator
#
#
######################################################################
# Machine specific stuff
#
#
LIBRARY = $(LIBDIR)/libHP.a

#
# Compile time resolution.
#
EXT_CFLAGS +=  -std=gnu++11 -g

INCLUDE =  -I$(COMMON)/GPIB -I$(COMMON)/utility 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = HP3314.cpp
SRCS    = $(SRC) $(SRCCPP)

HEADERS = HP3314.hh Version.hh
# When we build all, what do we build?
all:      $(LIBRARY)

#
# The dictionaries are generated automatically using the rootcint
# program. They are based on the headers and do some magic
# with respect to the ability of root to call my local classes.
# This is a Qt thing, so if you want to know more about it
# look at the Qt documentation.
#
#S_Dict.cpp: 	$(HEADERS)
#	@echo "Generating builder root dictionary $@..."
#	rootcint -f $@ -c $(HEADERS)

include $(DRIVE)/common/makefiles/makefile.inc


#dependencies
#include make.depend 
# DO NOT DELETE
