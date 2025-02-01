# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -g -Iinclude 

# Optimization level
OPTIMIZATION_LEVEL = -O3   # Change this to -Og, -O2, -O3, or -Ofast

# SFML libraries to link
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Source and header directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
APPLICATION_DIR = application

# Files to ignore
IGNORE = $(SRC_DIR)/filename.cpp

# Automatically find all .cpp files in the src directory and exclude ignored files
SRCS = $(filter-out $(IGNORE), $(wildcard $(SRC_DIR)/*.cpp))
OBJS_IN_BUILD_DIR = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
EXEC = $(APPLICATION_DIR)/main

# Default target
all: clean $(EXEC)

# Force rebuild by adding .PHONY target
.PHONY: all clean

# Build executable
$(EXEC): $(OBJS_IN_BUILD_DIR) | $(APPLICATION_DIR)
	$(CXX) $(OBJS_IN_BUILD_DIR) -o $(EXEC) $(LIBS)

# Compile object files with optimization
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(OPTIMIZATION_LEVEL) -c $< -o $@

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create application directory
$(APPLICATION_DIR):
	mkdir -p $(APPLICATION_DIR)

# Clean rule
clean:
	rm -rf $(BUILD_DIR) $(APPLICATION_DIR)

run: $(EXEC)
	$(EXEC)
