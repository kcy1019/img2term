CC=g++
CFLAGS=-o img2term -O3 -std=c++11 `Magick++-config --cppflags --cxxflags --ldflags --libs` -lncurses

all:
	$(CC) main.cc $(CFLAGS)

clean:
	rm -f img2term ascii.gch xterm.gch metric.gch image.gch
