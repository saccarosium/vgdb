CC = gcc
CFLAGS += -I./include -std=c99 -Wall -Wextra -Werror
CFLAGS += -Wno-missing-field-initializers -Wno-unused-parameter -Wno-sign-compare -Wno-cast-function-type

ifeq ($(DEBUG), 1)
    CFLAGS += -DDEBUG -O0 
	OBJDIR = ./build/debug
else
    CFLAGS += -DNDEBUG -O3
	OBJDIR = ./build/release
endif

LIB = $(OBJDIR)/libglfw3.a

SOURCES = ./src/context.c\
		  ./src/init.c\
		  ./src/input.c\
		  ./src/monitor.c\
		  ./src/platform.c\
		  ./src/vulkan.c\
		  ./src/window.c\
		  ./src/egl_context.c\
		  ./src/osmesa_context.c\
		  ./src/null_init.c\
		  ./src/null_monitor.c\
		  ./src/null_window.c\
		  ./src/null_joystick.c

# linux, BSD, etc.
CFLAGS += -D_GLFW_X11 -D_POSIX_C_SOURCE=200809L
LIBS += -lpthread -lm -ldl -lGL -lX11
SOURCES += ./src/posix_module.c\
			./src/posix_time.c\
			./src/posix_thread.c\
			./src/x11_init.c\
			./src/x11_monitor.c\
			./src/x11_window.c\
			./src/xkb_unicode.c\
			./src/glx_context.c\
			./src/posix_poll.c\
			./src/linux_joystick.c

OBJS = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))

all: | $(LIB)
	
$(LIB): $(OBJS)
	ar rcs $@ $^

$(OBJDIR)/%.o:./src/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(OBJDIR)/%.o:./src/%.m
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(OBJS): | $(OBJDIR)

$(OBJDIR):
	mkdir -p $@

clean:
	rm -f $(LIB) $(OBJS)


