# Makefile adapted from Zed Shaw's book,
# Learn C the Hard Way, Ex. 28 (pg 153-154)

CFLAGS = -Wall -Wextra -Iinclude $(OPTFLAGS)
LDFLAGS = $(OPTLIBS)
LDLIBS = -lcheck -lm -pthread -lrt -lsubunit

SOURCES = $(wildcard src/**/*.c src/*.c)
OBJECTS = $(patsubst src/%.c,build/%.o,$(SOURCES))

TEST_SRC = $(wildcard tests/*_tests.c)
TESTS = $(patsubst %.c,%,$(TEST_SRC))

TARGET = lib/libyahi.a

all: $(TARGET) tests

.PHONY: build
build:
	mkdir -p build
	mkdir -p bin

# This will probably break if subdirectories to build and src
# are introduced. Just a temporary bodge to get make working
# so I can start coding again... this restructure has taken
# quite a long time as it is! 
build/%.o: src/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) $(LDLIBS) -c $< -o $@

$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

.PHONY: tests
tests: LDLIBS += $(TARGET)
tests: $(TESTS)
	sh ./tests/unit-tests.sh

clean:
	rm -rf $(TARGET)
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	rm -rf tests/testdb
