CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -Iinclude/
# CFLAGS = -std=c11 -O3 -Iinclude/
BUILD_DIR = build
BIN_DIR = .

all: pre_conditions program. vector. trie. config. main hash

%.:
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$@o -c source/$@c

main:
	$(CC) $(CFLAGS) -o $(BIN_DIR)/uwc.out source/main.c $(BUILD_DIR)/*.o

hash:
	$(CC) $(CFLAGS) -o $(BIN_DIR)/hash.out source/hash.c $(BUILD_DIR)/*.o

clean:
	rm -f bin/* ; rm -f $(BUILD_DIR)/* ;

pre_conditions:
	mkdir -pv $(BUILD_DIR) $(BIN_DIR) ;
