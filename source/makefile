SRC_DIR=./
OBJ_DIR=obj

SOURCES=pch.cpp \
main.cpp \
CommandManager.cpp \
../common/serialport.cpp

INCLUDES=-I./ -I../common -I../thirdparty/curl/include
EXECUTABLE=hello

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
LDLIBS=-L../thirdparty/curl/ubuntu/ -lcurl

# auto ------------------------------------------------------------------------

empty :=
space := $(empty) $(empty)

#OBJECTS=$(SOURCES:.cpp=.o)
#OBJECTS_TEMP = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SOURCES))
#OBJECTS = $(subst ..,__,$(OBJECTS_TEMP))
OBJECTS := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(notdir $(SOURCES)))

#SRCDIRS = $(shell find . -name '*.cpp' | dirname {} | sort | uniq | sed 's/\/$(SOURCES)//g' )
SRCDIRS_RAW = $(shell dirname $(SOURCES) | sort | uniq )
SRCDIRS_OBJ = $(subst ..,__,$(SRCDIRS_RAW))
VPATH = $(subst $(space),:,$(SRCDIRS_RAW))

# targets ---------------------------------------------------------------------

all: buildrepo $(SOURCES) $(EXECUTABLE)

clean:
	rm -rf $(OBJ_DIR)

# link	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@ $(LDLIBS)

# compilation
$(OBJ_DIR)/%.o : %.cpp
	@echo compile $<
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

buildrepo:
	@$(call make-repo)

# functions -------------------------------------------------------------------

# Create obj directory structure
define make-repo
#	@echo "DUMP"
#	@echo $(SOURCES)
#	@echo $(SRCDIRS)
#	@echo $(OBJECTS)
#	@echo "HHH"
	mkdir -p $(OBJ_DIR)
#	for dir in $(SRCDIRS_OBJ); \
#	do \
#		mkdir -p $(OBJ_DIR)/$$dir; \
#	done
endef
