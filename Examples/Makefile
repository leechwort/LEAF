CFLAGS := -Wall -Wextra -fPIC -I../leaf/Inc
LDFLAGS := -L../leaf -lleaf -lm -lsndfile


all: \
	basic_oscillator

basic_oscillator: basic-oscillators.c
	gcc $< $(CFLAGS) $(LDFLAGS) -o $@ 
clean:
	rm -rf basic_oscillator

.PHONY: all clean

