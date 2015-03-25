#include"ascii.hxx"
#include"metric.hxx"
#include"xterm.h"
#include"image.hxx"
#include<ncurses.h>
#include<vector>
#include<string>
#include<tuple>
#include<cstdint>
#include<cstdlib>
#include<cstring>
#include<unistd.h>
using namespace std;

int cols, lines;

WINDOW* InitCurses(void)
{
	putenv("TERM=xterm-256color");
	WINDOW *ret = initscr();
	getmaxyx(ret, lines, cols);
	nodelay(ret, TRUE);
	keypad(stdscr, TRUE);
	timeout(0);
	start_color();
	for (int i = 0; i < COLORS; i++)
		init_pair(i, i, 0);
	return ret;
}

const int STDOUT(1);
int main(int argc, char *argv[])
{
	const char usage[] =
		"[38;5;11mimg2term - convert image into terminal ascii art.[38;5;15m\n"
		"usage1: img2term (filename|url) [38;5;14m[-z|--zoom] [38;5;10m# print to screen[38;5;15m\n"
		"usage2: img2term (filename|url) [38;5;9m> my_img.sh [38;5;10m# convert the image into shell script\n"
		"        the script can be used by typing bash my_img.sh[38;5;7m";

	if (argc < 2) {
		puts(usage);
		return 0;
	}

	bool is_zoom_mode = argc > 2 &&
						((string)argv[2] == "-z" ||
						 (string)argv[2] == "--zoom");

	InitCurses();
	ImageLoader *loader = ImageLoader::get_instance(argv[0]);
	vector<vector<vector<uint32_t>>> img;;
	vector<uint32_t> delay;

	if (!is_zoom_mode) {
		tie(delay, img) = loader->load(argv[1], cols/2, lines);
	} else {
		tie(delay, img) = loader->load(argv[1], cols/2, 1 << 16);
	}

	if (!isatty(STDOUT)) {
		/* stdout is redirected to non-terminal output.
		 * print as bash script format */
		vector<vector<string>> asc = IMG2ASCII(img).convert();
		uint32_t fptr = 0;
		printf("export TERM=xterm-256color\n");
		printf("clear\n");
		for (auto& frm: asc) {
			uint32_t rptr = 1;
			for (auto& row: frm) {
				printf("echo \"[%d;1H\"\n", rptr);
				printf("echo \"%s\"\n", row.c_str());
				++rptr;
			}
			printf("sleep %.2f\n", delay[fptr] / 100.);
			++fptr;
		}
		clear();
		refresh();
		endwin();
		return 0;
	}

	clear();
	refresh();

	vector<vector<string>> asc = IMG2ASCII(img).convert_raw();
	int zoom_ptr = 0;
	for (;;) {
		uint32_t fptr = 0;
		for (auto& frm: asc) {
			int rptr = 0, cptr = 0;
			for (int r = zoom_ptr; r < frm.size(); r++) {
				auto& row = frm[r];
				for (cptr = 0; cptr < row.size(); cptr++) {
					int col = (unsigned char)row[cptr];
					attrset(COLOR_PAIR(col));
					mvprintw(rptr, cptr*2, "##");
				}
				++rptr;
			}
			attrset(COLOR_PAIR(0));
			mvprintw(min(rptr, lines-1),  0, "Press q(Q) to quit");
			mvprintw(min(rptr, lines-1), 18, ", and press arrow keys to move");
			refresh();
			usleep(10000 * delay[fptr]);
			++fptr;
		}
		int ch = getch();
		if (ch == KEY_DOWN) {
			if (-zoom_ptr + (int)asc[0].size() > lines)
				++zoom_ptr;
		} else if (ch == KEY_UP) {
			if (zoom_ptr > 0)
				--zoom_ptr;
		} else if (ch == 'q' || ch == 'Q') {
			break;
		}
	}

	endwin();

	return 0;
}
