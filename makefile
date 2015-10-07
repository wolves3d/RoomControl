#Globals
BIN_DIR := ./bin
OBJ_TARGET_DIR := ./obj

#inputs
CLIENT_SRC_DIRS := source/*.c* \
source/Arduino/*.c* \
source/Client/*.c* \
source/Server/*.c* \
thirdparty/pugixml/*.c* \
submodules/CodeBase/network/**/*.c*

#Client source dirs
#CLIENT_SRC_DIRS := . Arduino Client Server
#CLIENT_SRC_DIRS := .
CLIENT_INC_DIRS := source submodules thirdparty/curl/include thirdparty/mysql/include thirdparty/pugixml

# Add source path prefix
CLIENT_INC_DIRS := $(addprefix -I, $(CLIENT_INC_DIRS))
# Search *.c* files
CLIENT_SRC := $(wildcard $(CLIENT_SRC_DIRS))
#CLIENT_SRC := $(subst //.//,//,$(CLIENT_SRC))


#CLIENT_OBJ := $(patsubst ../../%, %, $(wildcard $(addsuffix /*.c*, $(relative_source_dirs))))
CLIENT_OBJ := $(CLIENT_SRC)
CLIENT_OBJ := $(CLIENT_OBJ:.cpp=.o)
CLIENT_OBJ := $(CLIENT_OBJ:.c=.o)
CLIENT_OBJ_LIST := $(addprefix $(OBJ_TARGET_DIR)/, $(CLIENT_OBJ))


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

clean:
	rm -rf $(OBJ_TARGET_DIR)
	rm -rf $(BIN_DIR)

show_vars:
	@$(call dump-vars)
	
client: obj_dirs $(CLIENT_OBJ) client_link

client_link:
	@echo Linking
	$(CC) $(LDFLAGS) $(CLIENT_OBJ_LIST) -o $(BIN_DIR)/RoomControlClient $(LDLIBS)

obj_dirs:
	@echo Creating output dirs
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(addprefix $(OBJ_TARGET_DIR)/, $(dir $(CLIENT_OBJ)))

%.o : %.cpp
	@echo '    ' Compiling $<
	@$(CC) $(CLIENT_INC_DIRS) $(CFLAGS) -c $< -o $(OBJ_TARGET_DIR)/$@

# functions -------------------------------------------------------------------

define dump-vars
	@echo "OS:$(OS_NAME) Platform:$(PLATFORM)"
#	@echo "CLIENT_SRC"
#	@echo $(CLIENT_SRC)
#	@echo "CLIENT_SRC_DIRS"
#	@echo $(CLIENT_SRC_DIRS)
#	@echo "CLIENT_SRC_DIRS"
#	@echo $(CLIENT_SRC_DIRS)
#	@echo "CLIENT_INC_DIRS"
#	@echo $(CLIENT_INC_DIRS)
#	@echo "CLIENT_OBJ"
#	@echo $(CLIENT_OBJ)
endef
