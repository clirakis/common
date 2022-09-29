##################################################################
#
#	Makefile for libio using gcc on IRIX. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	16-Sep-00       CBL     Original
#       05-Nov-01       CBL 	Modified so that we can use either
#                               Linux or IRIX transparently. 
#                               ALSO: moved all common makefile stuff
#                               to ../makefiles/makefile.inc
#
######################################################################
#
LIBRARY = $(LIBDIR)/libio.a
LIBS = 
INCLUDE = -I../utility

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = SerialIO.cpp sharedmem.cpp netIO.cpp SharedMem2.cpp
HEADERS = SerialIO.h sharedmem.hh netIO.hh SharedMem2.hh

# When we build all, what do we build?
all:      $(LIBRARY)
include ../makefiles/makefile.inc

#dependencies
include make.depend 
# DO NOT DELETE
