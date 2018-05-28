bin := pannellum
src := main.c glad.c texture_loader.c\
    config.c\
    context/context.c\
    cjson/cJSON.c\
    renderer/multires.c\
    utils/log.c utils/stb_image.c utils/format_string.c utils/json_parse.c utils/args.c\
    gl_utils/texture.c gl_utils/shader.c gl_utils/gl_error.c

CFLAGS   := -Wall -g -Iinclude -std=c11 # -DLOG_LEVEL=0
CFLAGS-DEBUG   := -O0 -DDEBUG
CFLAGS-PROFILE := ${CFLAGS-DEBUG} -pg
CFLAGS-RELEASE := -O2
LFLAGS   :=
LFLAGS-DEBUG :=
LFLAGS-PROFILE := -pg
LFLAGS-RELEASE :=
LIBS     := -lm -ldl
packages := gl glfw3
target ?= release

LOG_LEVEL ?= LOG_DEBUG # can be LOG_DEBUG/WARN/INFO/ERROR/NONE
CFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)

sources := $(addprefix src/,$(src)) 
objects := $(addprefix build/,$(sources:.c=.o))

$(info Target ${target})
CFLAGS += $(CFLAGS-${target})
LFLAGS += $(LFLAGS-${target})

ifneq ($(packages),)
    LIBS    += $(shell pkg-config --libs-only-l $(packages))
    LFLAGS  += $(shell pkg-config --libs-only-L --libs-only-other $(packages))
    CFLAGS  += $(shell pkg-config --cflags $(packages))
endif



.PHONY: all clean

all: $(bin)
clean:
	-rm -rf build
	-rm -f $(bin)

$(bin): Makefile $(objects)
	$(CC) $(LFLAGS) -o $@ $(objects) $(LIBS)

build/%.o: %.c Makefile
	-mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -MMD -o $@ $<

-include $(addprefix build/,$(sources:.c=.d))

