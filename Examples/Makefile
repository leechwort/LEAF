CFLAGS := -Wall -Wextra -g -fPIC -I../leaf/Inc -I./
LDFLAGS := -L../leaf -lleaf -lm -lsndfile


all: \
	basic_oscillator \
	sawtooth_sequence

basic_oscillator: basic-oscillators.c samplewriter.c
	gcc $^ $(CFLAGS) $(LDFLAGS) -o $@ 

sawtooth_sequence: sawtooth-sequence.c samplewriter.c
	gcc $^ $(CFLAGS) $(LDFLAGS) -o $@ 
clean:
	rm -rf basic_oscillator sawtooth_sequence output.wav

.PHONY: all clean

