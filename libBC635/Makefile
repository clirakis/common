##################################################################
#
#	Makefile for libBC635 using gcc
#
#
#	Modified		by		Reason
# 	--------		--		------
#	06-Feb-11       CBL     Original
#
######################################################################
#
COMMON  = $(HOME)/common
LIBRARY = $(LIBDIR)/libBC635.a
LIBS = 
INCLUDE = -I$(COMMON)/utility -I$(HOME)/code/Time/BC635/Driver/v2.7b

# Rules to make the object files depend on the sources.
SRC     = bcuser.c
SRCCPP  = BC635.cpp
HEADERS = BC635.hh bcuser.h

EXT_CFLAGS =  -g 

# When we build all, what do we build?
all:      $(LIBRARY)
include $(COMMON)/makefiles/makefile.inc

#dependencies
#include make.depend 
# DO NOT DELETE
