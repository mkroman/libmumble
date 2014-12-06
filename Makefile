# Installation prefix.
ifdef PREFIX
	PREFIX=/usr
endif

# Compiler flags.
CFLAGS += -Wall -Werror -pedantic -std=c99 -Iinclude

# Provide definitions for current library version.
VERSION = $(shell git describe --tags --abbrev=0)
GIT_VERSION := "$(shell git describe --tags --always) ($(shell git log --pretty=format:%cd --date=short -n1))"
OS := $(shell uname)

ifeq ($(OS),Linux)
	CFLAGS += -DLINUX
endif

V ?= 0
ifeq ($(V),0)
# Don't print command lines which are run
.SILENT:
endif

CFLAGS += -DVERSION=\"${GIT_VERSION}\"

OBJS := $(wildcard src/*.c)
OBJS := $(OBJS:.c=.o)

src/%.o: src/%.c
	@echo " CC $<"
	$(CC) $(CFLAGS) -c -o $@ $<

src/Mumble.o: proto/Mumble.pb-c.c
	@echo " CC $<"
	$(CC) -c $< -I. -lprotobuf-c -o $@

proto/Mumble.pb-c.c:
	@echo " PROTOC $<"
	protoc-c --c_out=. proto/Mumble.proto

client: ${OBJS}
	@echo " LD $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

default: client

clean:
	rm -f src/*.o

distclean: clean
	rm -f client
