SRC_DIR=./
MODULE_DIR=../submodules/CodeBase/network
OBJ_DIR=obj

SOURCES := $(wildcard *.cpp) $(wildcard **/*.cpp) $(wildcard $(MODULE_DIR)/**/*.cpp)

#SOURCES=pch.cpp \
#main.cpp \
#Client/Client.cpp \
#Client/ClientRequests.cpp \
#Client/ClientResponses.cpp

INCLUDES=-I./ \
-I../submodules \
-I../thirdparty/curl/include \
-I../thirdparty/mysql/include


EXECUTABLE=hello

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
LDLIBS=\
-L../thirdparty/curl/ubuntu \
-L../thirdparty/mysql/lib/ubuntu/x86_32 \
-lcurl -lmysqlclient

# auto ------------------------------------------------------------------------
#http://stackoverflow.com/questions/7004702/how-can-i-create-a-makefile-for-c-projects-with-src-obj-and-bin-subdirectories

empty :=
space := $(empty) $(empty)

OBJECTS=$(SOURCES:.cpp=.o)
LOBJECTS := $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
#OBJECTS := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
#LOBJECTS=$(SOURCES:.cpp=.o)
#LOBJECTS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SOURCES)))

#SRCDIRS = $(shell find . -name '*.cpp' | dirname {} | sort | uniq | sed 's/\/$(SOURCES)//g' )
#SRCDIRS_RAW = $(shell dirname $(SOURCES) | sort | uniq )
#SRCDIRS_RAW = $(shell dirname $(SOURCES))
#SRCDIRS_OBJ = $(subst ..,__,$(SRCDIRS_RAW))
VPATH = $(subst $(space),:,$(SRCDIRS_RAW))

# targets ---------------------------------------------------------------------

all: buildrepo $(SOURCES) $(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR)

# link	
$(EXECUTABLE): $(OBJECTS) 
	@echo Linknig
	$(CC) $(LDFLAGS) $(LOBJECTS) -o $@ $(LDLIBS)

# compilation
%.o : %.cpp
	@echo compile $<
	@echo output $@
	$(CC) $(INCLUDES) $(CFLAGS) -c $< -o $(OBJ_DIR)/$@

buildrepo:
	@$(call make-repo)

# functions -------------------------------------------------------------------

# Create obj directory structure
define make-repo
	@echo "SOURCES"
	@echo $(SOURCES)
	@echo "OBJECTS"
	@echo $(OBJECTS)
	@echo "LOBJECTS"
	@echo $(LOBJECTS)
	@echo "HHH"
	mkdir -p $(OBJ_DIR)
	@echo "ZZZ"
	for F in $(SOURCES);\
	do\
		echo $$F;\
		echo $$(dirname $$F);\
		mkdir -p $(OBJ_DIR)/$$(dirname $$F);\
	done
endef
