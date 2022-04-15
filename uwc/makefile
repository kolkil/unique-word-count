CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude/

# define you structure here
BUILD_DIR = build
BIN_DIR = bin
SOURCE_DIR = source

# define your executables - files with main function
EXEC := uwc.c hash.c strip.c split.c strip_nonwords.c

# trick to filter executables from objects
EXEC_LIST = $(patsubst %, *%, $(EXEC))

# filter executables from objects
# first find the sources and exclude executables (those with main function)
# then replace extension .c with .o, then replace part of path SOURCE_DIR with BUILD_DIR
OBJECTS := $(patsubst $(SOURCE_DIR)%, $(BUILD_DIR)%, $(patsubst %.c, %.o, $(shell find $(SOURCE_DIR) -name *.c $(shell printf "! -name %s " $(EXEC_LIST)))))

# first replace star in EXEC_LIST with BIN_DIR
# then replace .c with .out
EXECUTABLES := $(patsubst %.c, %.out, $(patsubst *%, $(BIN_DIR)/%, $(EXEC_LIST)))

all: pre_conditions $(OBJECTS) $(EXECUTABLES)

.PHONY: $(OBJECTS)
.PHONY: $(EXECUTABLES)

$(OBJECTS) : $(BUILD_DIR)/%.o : $(SOURCE_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $?

$(EXECUTABLES) : $(BIN_DIR)/%.out : $(SOURCE_DIR)/%.c | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $? $(OBJECTS)

log:
	echo $(EXECUTABLES)
	echo $(OBJECTS)

clean:
	rm -f $(BIN_DIR)/* ; rm -f $(BUILD_DIR)/* ;

pre_conditions:
	mkdir -pv $(BUILD_DIR) $(BIN_DIR) ;
