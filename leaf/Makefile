CC := gcc
CFLAGS := -Wall -Wextra -g -IExternals -fPIC
LDLIBS := -lm

SRC_DIR := Src
OBJ_DIR := obj
LIB_NAME := libleaf.a

SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
D_FFT_OBJ = $(wildcard Externals/*.o)

# Targets
all: $(LIB_NAME)

$(LIB_NAME): $(OBJS) $(D_FFT_OBJ)
	ar -rcs -o  $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(D_FFT_OBJ) $(LIB_NAME)

.PHONY: all clean

