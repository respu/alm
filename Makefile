#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = g++

# define any compile-time flags
CFLAGS = -Wall -g -std=c++0x -shared

# define any directories containing header files other than /usr/include
#
INCLUDES = -Iinclude

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS =

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = 

# define the C source files
SRCDIR = src
EXT = cpp
SOURCES = $(wildcard $(SRCDIR)/*.$(EXT))

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .cpp of all words in the macro SOURCES
# with the .o suffix
#
OBJDIR = obj
OBJS1 = $(SOURCES:.$(EXT)=.o)
OBJS2 =  $(notdir $(OBJS1))
OBJS =  $(patsubst %,$(OBJDIR)/%,$(OBJS2))

# define the executable file 
BINDIR = lib
BIN = $(BINDIR)/libalm.so

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all:	MAKE_DIR	\
	$(BIN)

MAKE_DIR:
	if [ ! -d $(OBJDIR) ];then	\
		mkdir $(OBJDIR);	\
	fi
	if [ ! -d $(BINDIR) ];then	\
		mkdir $(BINDIR);	\
	fi

$(BIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .cpp's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .cpp file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
$(OBJDIR)/%.o: $(SRCDIR)/%.$(EXT) 
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

build.all:	all		\
		build.test	\
		build.examples

build.test:
	cd test;		\
	make;	


build.examples:
	cd examples/canvas_server;		\
	make;
	cd examples/http_server;		\
	make;
	cd examples/http_server;		\
	make;
	cd examples/tasks_processor/client;	\
	make;
	cd examples/tasks_processor/server;	\
	make;
	cd examples/websocket_server;		\
	make;

clean:
	$(RM) $(OBJDIR)/*.o *~ $(BIN)


clean.all:	clean		\
		clean.test	\
		clean.examples

clean.test:
	cd test;		\
	make clean;

clean.examples:
	cd examples/canvas_server;		\
	make clean;
	cd examples/http_server;		\
	make clean;
	cd examples/http_server;		\
	make clean;
	cd examples/tasks_processor/client;	\
	make clean;
	cd examples/tasks_processor/server;	\
	make clean;
	cd examples/websocket_server;		\
	make clean;


depend: $(SOURCES)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
