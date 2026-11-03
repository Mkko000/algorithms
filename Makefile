# Algorithms in C — Mohammed El Sisi
# Based on / adapted from R. Sedgewick, Algorithms in C (3rd ed.), Parts 1-4
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -O2
LDFLAGS =

SOURCES := $(sort $(wildcard src/*/*.c src/*/exercises/*.c))
BINDIR  = bin
TARGETS = $(patsubst src/%.c,$(BINDIR)/%,$(SOURCES))

.PHONY: all c run test check-c clean

all: c

c: $(TARGETS)

$(BINDIR)/%: src/%.c src/common_item.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -lm

# Files that don't need common_item.h still match the pattern via a second rule
$(BINDIR)/%: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS) -lm

clean:
	rm -rf $(BINDIR)

# Run every binary; stop on first nonzero exit.
run: c
	@fail=0; \
	for b in $(TARGETS); do \
	  echo "==== $$b ===="; \
	  "$$b" || fail=1; \
	done; \
	exit $$fail

test: run

check-c:
	$(MAKE) c CFLAGS="$(CFLAGS) -Werror"
	$(MAKE) run
