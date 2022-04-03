CC = g++

UNAME_S = $(shell uname -s)
BIN = bin

# Amount of cores to use when building
PROCESSER_COUNT = 8

IMGUI_DIR = lib/imgui
IMGUI_NODE_EDITOR_DIR = lib/imgui-node-editor

INCFLAGS  = -Iinclude
INCFLAGS += -Ilib/bgfx/include
INCFLAGS += -Ilib/bx/include
INCFLAGS += -Ilib/bimg/include
INCFLAGS += -Ilib/glfw/include
INCFLAGS += -I$(IMGUI_DIR)
INCFLAGS += -I$(IMGUI_DIR)/backends
INCFLAGS += -I$(IMGUI_NODE_EDITOR_DIR)

CCFLAGS  = -std=c++20 -O2
CCFLAGS += $(INCFLAGS)

# -lm = libm = math lib
LDFLAGS  = -lm -lGL -lX11 -ldl -lpthread -lrt -lvulkan
LDFLAGS += $(INCFLAGS)

# To be set later in makefile
BGFX_TARGET =
BGFX_DEPS_TARGET =

# TODO: Support Windows in the future??
ifeq ($(UNAME_S), Linux)
	BGFX_DEPS_TARGET = linux64_gcc
	BGFX_TARGET = linux
endif

CPP_SOURCES	 = $(wildcard src/*.cpp)
IMGUI_SOURCES	 = $(wildcard $(IMGUI_DIR)/*.cpp)
IMGUI_NODE_EDITOR_SOURCES = $(wildcard $(IMGUI_NODE_EDITOR_DIR)/*.cpp)
# Has to use wildcard otherwise errors
IMGUI_BACKEND_SOURCES = $(wildcard $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_vulkan.cpp)
OBJ		 = $(CPP_SOURCES:.cpp=.o)
OBJ		+= $(IMGUI_SOURCES:.cpp=.o)
OBJ		+= $(IMGUI_BACKEND_SOURCES:.cpp=.o)
OBJ		+= $(IMGUI_NODE_EDITOR_SOURCES:.cpp=.o)

BGFX_BIN = lib/bgfx/.build/$(BGFX_DEPS_TARGET)/bin
BGFX_CONFIG = Debug

ifeq ($(BGFX_CONFIG), Debug)
	CCFLAGS += -DSHADOW_DEBUG_BUILD
	CCFLAGS += -DBX_CONFIG_DEBUG
endif

LDFLAGS += -lstdc++
LDFLAGS += $(BGFX_BIN)/libbgfx$(BGFX_CONFIG).a
LDFLAGS += $(BGFX_BIN)/libbimg$(BGFX_CONFIG).a
LDFLAGS += $(BGFX_BIN)/libbx$(BGFX_CONFIG).a
LDFLAGS += lib/glfw/src/libglfw3.a

SHADER_TARGET	= vulkan
SHADER_PLATFORM	= linux
SHADERS_PATH	= res/shaders
SHADERS		= $(shell find $(SHADERS_PATH)/* -maxdepth 1 | grep -E ".*/(vs|fs).*.sc")
SHADERS_OUT	= $(SHADERS:.sc=.$(SHADER_TARGET).bin)
SHADERC		= lib/bgfx/.build/$(BGFX_DEPS_TARGET)/bin/shaderc$(BGFX_CONFIG)

.PHONY: all clean runtimeres

all: dirs libs shaders build

#	cd lib/bx && make -j$(PROCESSER_COUNT) $(BGFX_TARGET)
libs:
	cd lib/bimg && make -j$(PROCESSER_COUNT) $(BGFX_TARGET)
	cd lib/bgfx && make -j$(PROCESSER_COUNT) $(BGFX_TARGET)
	cd lib/glfw && cmake . && make -j $(PROCESSER_COUNT)

dirs:
	mkdir -p ./$(BIN)

runtimeres:
	cp runtimeres/* bin

# Shader -> bin
%.$(SHADER_TARGET).bin: %.sc
	$(SHADERC)	--type $(shell echo $(notdir $@) | cut -c 1) \
			-i lib/bgfx/src \
			--platform $(SHADER_PLATFORM) \
			-f $< \
			-o $@

shaders: $(SHADERS_OUT)

run: build
	cd $(BIN)
	$(shell ./game)

build: runtimeres dirs shaders $(OBJ)
	$(CC) -o $(BIN)/game $(filter %.o,$^) $(LDFLAGS)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

%.o: $(IMGUI_DIR)/%.cpp
	$(CC) -o $@ -c $< $(CCFLAGS)

clean:
	find res/shaders -name "*.bin" -delete
	rm -rf $(BIN) $(OBJ)
	rm -rf lib/glfw/CMakeCache.txt
