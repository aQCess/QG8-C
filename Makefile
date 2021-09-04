#
# Makefile
#
# Author       : Finn Rayment <finn@rayment.fr>
# Date created : 27/04/2021
#

# set DEBUG to 1 to turn on verbose messages
DEBUG:=0

# set USE_GSL to 1 to enable build of extra functions that rely on GSL
# -- not currently implemented -- #
USE_GSL:=0

PREFIX:=/usr/local
VERSION:=1.0.0

CC:=cc
CCFLAGS:=--std=c89# -Wno-error=stringop-truncation
#-fsanitize=address -fno-omit-frame-pointer
CXXFLAGS:=-Wall -Wextra -Wpedantic -Werror --pedantic-errors \
          -fPIC -O3 -DVERSION=\"${VERSION}\"
INCLUDE:=-Iinclude
ifeq ($(DEBUG),1)
CXXFLAGS+=-g -DDEBUG=1
endif
ifeq ($(USE_GSL),1)
CXXFLAGS+=-DUSE_GSL=1
LIBRARIES:=$(shell pkg-config --libs gsl)
INCLUDE+=$(shell pkg-config --cflags gsl)
else
LIBRARIES:=
endif

LDFLAGS:=$(LIBRARIES) -lm

SOURCES:=$(wildcard src/*.c) $(wildcard src/*/*.c)
OBJECTS:=$(patsubst src/%,obj/%,\
         $(patsubst %.c,%.o,$(SOURCES)))
HEADERS:=include/*.h

STATIC_LIB:=libqg8.a.$(VERSION)
LINK_LIB:=libqg8.so
DYNAMIC_LIB:=$(LINK_LIB).$(VERSION)

TEST_SOURCES:=$(wildcard test/*/*.c)
TEST_BINARIES:=$(patsubst %.c,%,$(TEST_SOURCES))
TEST_INCLUDE:=-Itest

all: $(OBJECTS)
	mkdir -p build
	ar -crv build/$(STATIC_LIB) $(OBJECTS)
	ranlib build/$(STATIC_LIB)
	$(CC) $(OBJECTS) -shared -o build/$(DYNAMIC_LIB)
	ln -fs $(DYNAMIC_LIB) build/$(LINK_LIB)

obj/%.o: src/%.c
	mkdir -p $(shell dirname $@)
	$(CC) $(CCFLAGS) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

archive:
	tar czvf libqg8-$(VERSION).tar.gz src/ include/ test/ README INSTALL COPYING Makefile

install:
	install -d $(PREFIX)/include/qg8
	install -m 644 $(HEADERS) $(PREFIX)/include/qg8
	install -d $(PREFIX)/lib
	cp -a -f build/$(LINK_LIB) $(PREFIX)/lib
	install build/$(DYNAMIC_LIB) $(PREFIX)/lib
	ldconfig

uninstall:
	rm -f $(PREFIX)/lib/$(LINK_LIB)
	rm -f $(PREFIX)/lib/$(DYNAMIC_LIB)
	rm -rf $(PREFIX)/include/qg8

test: $(TEST_BINARIES)

test/%: test/%.c
	$(CC) -g $(CCFLAGS) $(CXXFLAGS) $(INCLUDE) $(TEST_INCLUDE) $(OBJECTS) \
	$(LDFLAGS) $< -o $@

clean:
	rm -rf build obj $(BINARY) $(TEST_BINARIES)

.PHONY: all install uninstall test clean

