CXX = g++
CXXFLAGS += -I./third-party -I./src -I./third-party/glfw/include
CXXFLAGS += -std=c++23 -Wall -Wextra -Werror -pedantic

DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS += -DDEBUG -O0 -ggdb
	OBJDIR = build/debug
else
    CXXFLAGS += -DNDEBUG -O2
	OBJDIR = build/release
endif

BIN = $(OBJDIR)/tug

SAN ?= 0
ifeq ($(SAN), 1)
	CXXFLAGS += -fno-omit-frame-pointer -fsanitize=undefined,address
endif

IMGUI_DIR = ./third-party/imgui
SOURCES = ./src/main.cpp\
          $(IMGUI_DIR)/imgui.cpp\
          $(IMGUI_DIR)/imgui_draw.cpp\
          $(IMGUI_DIR)/imgui_impl_glfw.cpp\
          $(IMGUI_DIR)/imgui_impl_opengl2.cpp\
          $(IMGUI_DIR)/imgui_tables.cpp\
          $(IMGUI_DIR)/imgui_widgets.cpp

OBJS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

# GLFW
GLFW = glfw3
LIBS += -L ./third-party/glfw/$(OBJDIR) -l $(GLFW)
LIBS += -lpthread -lm -ldl -lGL -lX11

all: $(BIN)
	
$(BIN): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(BIN): | $(GLFW)

$(GLFW):
	DEBUG=$(DEBUG) $(MAKE) -C ./third-party/glfw

$(OBJDIR)/%.o:./src/%.cpp ./src/gdb.h ./src/common.h
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:./third-party/%.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c -o $@ $<

$(OBJDIR)/%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c -o $@ $<
	
$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

clean:
	rm -f $(BIN) $(OBJS)
	$(MAKE) -C ./third-party/glfw DEBUG=$(DEBUG) clean
