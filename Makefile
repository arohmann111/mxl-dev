TARGET   := 2110media-function
SRCS     := $(wildcard *.cpp)
BUILD_DIR := build
OBJS     := $(SRCS:%.cpp=$(BUILD_DIR)/%.o)

CXX      ?= g++
CXXFLAGS ?= -std=c++20 -g -O0 -Wall -Werror -Wextra
CPPFLAGS += $(shell pkg-config --cflags libmxl)
LDLIBS   += $(shell pkg-config --libs libmxl)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)
