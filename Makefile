# Compiler settings
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra
LDFLAGS := 
INCLUDES := -I.

# Project structure
SRC_DIR := .
BUILD_DIR := build
TARGET := gpsolve

# Debug vs Release builds
DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -O0
    BUILD_DIR := $(BUILD_DIR)/debug
else
    CXXFLAGS += -O3
    BUILD_DIR := $(BUILD_DIR)/release
endif

# Automatic source file discovery
# Find all .cpp files in the src directory and subdirectories
SRCS := $(shell find $(SRC_DIR) -name '*.cpp' ! -name 'main.cpp')
SRCS += $(SRC_DIR)/main.cpp

# Generate object file names
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Create directories for object files
DIRS := $(sort $(dir $(OBJS)))

# Main targets
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o $@

# Include dependency files
-include $(DEPS)

# Compilation rule with dependency generation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(DIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Create build directories
$(DIRS):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Print variables (for debugging)
print-%:
	@echo '$*=$($*)'