CPP = g++
CFLAGS = -Wall -g3 -O3 -std=c++11
K2R = tk2r.c
UTILS = utils.c
DACS = ./dacs/dacs.c ./dacs/bitrankw32int.c ./dacs/basics.c
SIMPLE_FAMILY = ./simple9/simple9.c ./simple16/simple16.c
PFORD = ./pford/coding_policy.c ./pford/pfordelta.c ./pford/pack.c ./pford/unpack.c 

TARGET = tk2r

all: k2rasterProg
	
k2rasterProg:
	$(CPP) -I ./libs/libcds2/includes -I ./include $(K2R) $(UTILS) $(DACS) $(SIMPLE_FAMILY) $(PFORD) -o $(TARGET) $(CFLAGS)

clean:
	rm tk2r utils ./dacs/dacs ./dacs/bitrankw32int ./dacs/basics
