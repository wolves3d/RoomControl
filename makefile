#Globals
SRC_ROOT_DIR := ./source
BIN_DIR := ./bin
OBJ_TARGET_DIR := ./obj
MODULE_DIR := ./submodules/CodeBase/network

#Client source dirs
CLIENT_SRC_DIRS := . Arduino Client
#CLIENT_SRC_DIRS := .
CLIENT_INC_DIRS := source submodules thirdparty/curl/include thirdparty/mysql/include

# Add source path prefix
CLIENT_SRC_DIRS := $(addprefix $(SRC_ROOT_DIR)/, $(CLIENT_SRC_DIRS))
CLIENT_INC_DIRS := $(addprefix -I, $(CLIENT_INC_DIRS))
# Search *.c* files
CLIENT_SRC := $(wildcard $(addsuffix /*.c*, $(CLIENT_SRC_DIRS)))
#CLIENT_SRC := $(subst //.//,//,$(CLIENT_SRC))


#CLIENT_OBJ := $(patsubst ../../%, %, $(wildcard $(addsuffix /*.c*, $(relative_source_dirs))))
CLIENT_OBJ := $(CLIENT_SRC)
CLIENT_OBJ := $(CLIENT_OBJ:.cpp=.o)
CLIENT_OBJ := $(CLIENT_OBJ:.c=.o)
CLIENT_OBJ_LIST := $(addprefix $(OBJ_TARGET_DIR)/, $(CLIENT_OBJ))

INCLUDES := -I./source \
-I../submodules \
-I../thirdparty/curl/include \
-I../thirdparty/mysql/include

OS_NAME := $(shell uname -o)
PLATFORM := $(shell uname -m)

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
LDLIBS=\
-Lthirdparty/curl/lib/$(PLATFORM) \
-Lthirdparty/mysql/lib/$(PLATFORM) \
-lcurl -lmysqlclient -lpthread

# auto ------------------------------------------------------------------------
#http://stackoverflow.com/questions/7004702/how-can-i-create-a-makefile-for-c-projects-with-src-obj-and-bin-subdirectories

comma := ,
empty :=
space := $(empty) $(empty)

OBJECTS=$(SOURCES:.cpp=.o)
LOBJECTST := $(SOURCES:%.cpp=$(OBJ_DIR)/%.o)
LOBJECTS := $(subst ..,__,$(LOBJECTST))

# targets ---------------------------------------------------------------------

all: show_vars client

show_vars:
	@$(call dump-vars)
	
client: obj_dirs $(CLIENT_OBJ)
	@echo Linking $<
	$(CC) $(LDFLAGS) $(CLIENT_OBJ_LIST) -o $(BIN_DIR)/$@ $(LDLIBS)

obj_dirs:
	mkdir -p $(BIN_DIR)
	mkdir -p $(addprefix $(OBJ_TARGET_DIR)/, $(dir $(CLIENT_OBJ)))

%.o : %.cpp
	@echo Compiling $<
	@$(CC) $(CLIENT_INC_DIRS) $(CFLAGS) -c $< -o $(OBJ_TARGET_DIR)/$@

# functions -------------------------------------------------------------------

define dump-vars
	@echo "os:$(OS_NAME) platform:$(PLATFORM)"
	
	@echo "CLIENT_SRC_DIRS"
	@echo $(CLIENT_SRC_DIRS)
	
	@echo "CLIENT_SRC_DIRS"
	@echo $(CLIENT_SRC_DIRS)
	
	@echo "CLIENT_INC_DIRS"
	@echo $(CLIENT_INC_DIRS)
	
	@echo "CLIENT_OBJ"
	@echo $(CLIENT_OBJ)
endef
