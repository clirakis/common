##################################################################
#
#	Makefile for tektronix DSA602 code using gcc on Linux. 
#
#
#	Modified	by	Reason
# 	--------	--	------
#	23-Jan-21       CBL     Original
#       18-Dec-22       CBL     Making a library. 
#       26-Dec-22       CBL     Moved MSLIST here
#
######################################################################
# Machine specific stuff
#
#
LIBRARY = $(HOME)/lib_Linux/libDSA602.a
#
# Compile time resolution.
#
#EXT_CFLAGS +=  -g 
# -DDEBUG_TRACE
# -DDEBUG_TB
# -DDEBUG_SAE=1

INCLUDE =  -I$(COMMON)/GPIB -I$(COMMON)/utility \
	-I/usr/local/include 

# Rules to make the object files depend on the sources.
SRC     = 
SRCCPP  = DSA602.cpp DSA602_Utility.cpp WFMPRE.cpp \
	Channel.cpp DSAFFT.cpp Module.cpp System.cpp \
	StatusAndEvent.cpp Measurement.cpp MeasurementA.cpp \
	TimeBase.cpp Trace.cpp AdjTrace.cpp Units.cpp GParse.cpp

SRCS    = $(SRC) $(SRCCPP)

HEADERS = Version.hh DSA602.hh WFMPRE.hh \
	Module.hh Channel.hh DSA602_Types.hh System.hh StatusAndEvent.hh \
	Measurement.hh MeasurementA.hh DSAFFT.hh TimeBase.hh Trace.hh \
	AdjTrace.hh Units.hh GParse.hh 

# When we build all, what do we build?
all:      $(LIBRARY)

include $(DRIVE)/common/makefiles/makefile.inc

#dependencies
include make.depend 
# DO NOT DELETE
