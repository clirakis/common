######################################################################
# File:      Makefile
# System:   To make the EMClass tools
# Created:  12-Sep-2003
# Author:   C.B. Lirakis
# 	    
# Remarks:
#
# COPYRIGHT 2002 BBN Inc.
# 127 John Clarke Rd.
# Middletown, R.I. 02842
# 401-848-3537
#
#
# $Header: /home/clirakis/common/EMTools/RCS/Makefile,v 1.2 2003/11/24 15:23:56 clirakis Exp clirakis $
#
#	modified	by	reason
#	--------	--	------
#
######################################################################
 
# environment variables needed by the system.
CC  = gcc
CPP = g++
LD  = gcc
AR  = ar
DEPEND = makedepend
RM = \rm -f
#
INCLUDE = -I../platform -I../utility
CFLAGS = $(EXT_CFLAGS) -Wall -O $(INCLUDE)
#
LIBPATH = 
LIBS    = 
LDFLAG  = $(LIBPATH) $(LIBS)

SOURCES  = emclass.cpp

LIBRARY = $(HOME)/lib_Linux/libemtools.a

TARGET = 


# Rules to make the object files depend on the sources.
OBJS    = ${SOURCES:.cpp=.o} 

# When we build all, what do we build?
all:      $(LIBRARY)

# Make depend rules
DEPEND = makedepend

# Create a file containing a list of dependencies.
make.depend: $(SOURCES)
	@ echo
	@ echo "Building make.depend file..."
	@ echo 
	echo "# DO NOT DELETE THIS LINE -- make depend depends on it." > make.depend ;\
	$(DEPEND) -f make.depend $(SOURCES) $(INCLUDE)

# Rules to convert from source to object files.
.SUFFIXES : .o .cpp .hpp .h .c $(SUFFIXES)

.cpp.o:
	$(CPP) $(CFLAGS) -c $<
.c.o:
	$(CPP) $(CFLAGS) -c $<

# Dependency for creating an executable
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS)  -o $@ $(OBJS)


$(LIBRARY): $(OBJS)
	ar -r $(LIBRARY) $(OBJS)
#	$(RANLIB) $(GTLIB)


#dependencies
#include make.depend

clean: 
	$(RM) ${OBJS} 
