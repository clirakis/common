##################################################################
#
#	Common makefile stuff, include this for all other makefiles.
#
#
#	Modified	by	Reason
# 	--------	--	------
#	05-Nov-01       CBL     Original
#                               Linux or IRIX transparently. 
#
#       19-May-05       CBL     Added in /usr/local/include for include path
#       10-Feb-06       CBL     Using cpp as preprocessor with M option for
#                               creating dependencies.
######################################################################
# Machine specific stuff
#
#
# Compilier defs.
#
CC  = gcc
CPP = g++
LD  = g++
AR  = ar
ARFLAGS = -r
#
# Other useful defines.
# 
COMMON=$(DRIVE)/common
CFLAGS = -Wall -DPOSIX $(INCLUDE) $(DEFINES) $(EXT_CFLAGS)
# LIBPATH defined externally for more stuff if necessary. 
LDFLAGS = -L$(LIBDIR) $(LIBPATH) $(LIBS) $(EXT_LDFLAGS) -lm -lpthread -lrt

# Rules to make the object files depend on the sources.
SRCS    = $(SRC) $(SRCCPP)

# Rules to make the object files depend on the sources.
OBJSC   = ${SRC:.c=.o}
OBJSCPP = ${SRCCPP:.cpp=.o}
OBJS    = $(OBJSC) $(OBJSCPP)

.cpp.o:
	$(CPP) $(CFLAGS) -c $<
.c.o:
	$(CC) $(CFLAGS) -c $<

# Rules to make dependencies. seems a little better
%.d: %.c
	$(CPP) $(CFLAGS) -M $< > $@
%.d: %.cpp
	$(CC) $(CFLAGS) -M $< > $@

DEPENDS   = ${OBJS:.o=.d} 

make.depend: $(DEPENDS)
	cat *.d > make.depend

$(LIBRARY): $(OBJS)
	$(AR) $(ARFLAGS) $(LIBRARY) $(OBJS)

$(TARGET): $(OBJS) 
	echo "External Compliation flags were $(EXT_CFLAGS) \n" 
	$(LD) $(OBJS) $(LDFLAGS) -o $(TARGET)

makedoc:
	doc++ -H -p -u -d ../html *.h

.PHONY : clean makedoc make.depend

clean:
	\rm -f *.d
	\rm -f *.o
	\rm -f make.depend
	\rm -f *.bak
	\rm -f *~
	\rm -f *Dict*
	\rm -f $(TARGET)

