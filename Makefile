# Algorithms in C — Mohammed El Sisi
# Based on / adapted from R. Sedgewick, Algorithms in C (3rd ed.), Parts 1-4
CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -O2
LDFLAGS =
SRCDIRS = src/01-fundamentals src/02-analysis src/03-data-structures \
          src/04-adt src/05-recursion-trees src/06-sorting src/07-quicksort \
          src/08-mergesort src/09-heapsort src/10-radix-sort src/11-special-sorts \
          src/12-bst src/13-balanced-trees src/14-hashing src/15-tries \
          src/16-external-search

SOURCES = $(foreach d,$(SRCDIRS),$(wildcard $(d)/*.c) $(wildcard $(d)/exercises/*.c))
BINDIR  = bin
TARGETS = $(patsubst src/%.c,$(BINDIR)/%,$(SOURCES))

.PHONY: all clean test

all: $(TARGETS)

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
test: all
	@fail=0; \
	for b in $(TARGETS); do \
	  echo "==== $$b ===="; \
	  "$$b" || fail=1; \
	done; \
	exit $$fail
