# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -fPIC -Iinclude -I. -Ielementary/runtime -std=c++17

# Source and object files
SRC_DIR = src
OBJ_DIR = obj
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Dynamic library
LIB_NAME = libmylib.dylib # macos only

# Default target
all: $(LIB_NAME)

# Rule to link object files to create the executable
$(LIB_NAME): $(OBJS)
	$(CXX) -dynamiclib $(OBJS) -o $@

# Rule to compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create object directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Clean target to remove generated files
clean:
	rm -rf $(OBJ_DIR) $(LIB_NAME)

# Phony targets
.PHONY: all clean
