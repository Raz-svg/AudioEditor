# --- Variables ---

# Compiler
CC = gcc

# Target Name
TARGET = sound_composer

# Source Files
SRCS = main.c util.c

# Object Files Directory
OBJDIR = ../build/obj

# Binary Output Directory
BINDIR = ../build/bin

# Object Files
OBJS = $(SRCS:.c=.o)
OBJS := $(addprefix $(OBJDIR)/, $(OBJS))

# Headers
HDRS = util.h

# Include Directories
INC_DIRS = -I./

# Library Directories
LIB_DIRS =

# Libraries
LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

# Compiler Flags
CFLAGS = -g -Wall -Wextra -std=c99 $(INC_DIRS)

# Linker Flags
LDFLAGS = $(LIB_DIRS) $(LIBS)

# Final Executable Path
OUT = $(BINDIR)/$(TARGET)

# --- Rules ---

# Default Target
all: $(OUT)

# Rule to Link the Executable
$(OUT): $(OBJS)
	@echo "Linking $(TARGET)..."
	@mkdir -p $(BINDIR)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "$(TARGET) built successfully at $(OUT)!"

# Rule to Compile .c to .o
$(OBJDIR)/%.o: %.c $(HDRS)
	@echo "Compiling $<..."
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean Up
clean:
	@echo "Cleaning up build files..."
	rm -rf $(OBJDIR) $(BINDIR)
	@echo "Cleanup complete."

# Run the Program
run: all
	@echo "Running $(TARGET)..."
	./$(OUT)

# Phony Targets
.PHONY: all clean run
