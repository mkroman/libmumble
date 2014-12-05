ifdef PREFIX
	PREFIX=/usr
endif

CFLAGS+=-Wall -Werror -pedantic -std=c99 -I./include

VERSION=$(shell git describe --tags --abbrev=0)
GIT_VERSION:="$(shell git describe --tags --always) ($(shell git log --pretty=format:%cd --date=short -n1))"
OS:=$(shell uname)

ifeq ($(OS),Linux)
	CFLAGS+=-DLINUX
endif

V ?= 0
ifeq ($(V),0)
# Don't print command lines which are run
.SILENT:
endif

CFLAGS+=-DVERSION=\"${GIT_VERSION}\"

OBJS:=$(wildcard src/*.c)
OBJS:=$(OBJS:.c=.o)

src/%.o: src/%.c src/Mumble.pb-c.c
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo " CC $<"

src/Mumble.pb-c.c:
	 protoc-c --c_out=. src/Mumble.proto

client: ${OBJS}
	@echo " LD $@"
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

static: ${OBJS}
	@echo " LD $@"
	$(CC) $(LDFLAGS) -static -o client $^ $(LIBS)

clean:
	rm -f src/*.o

distclean: clean
	rm -f client
