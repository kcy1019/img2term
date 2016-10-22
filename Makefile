CC=g++
CFLAGS=`pkg-config --cflags Magick++` -o img2term -O3 -std=c++11 `pkg-config --libs Magick++` -lncurses

all:
	$(CC) main.cc $(CFLAGS)

clean:
	rm -f img2term
