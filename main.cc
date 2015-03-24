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
	timeout(0);
	start_color();
	for (int i = 0; i < COLORS; i++)
		init_pair(i, i, 0);
	return ret;
}

int main(int argc, char *argv[])
{
	InitCurses();
	if (argc < 2) return puts("usage: ./img2term <filename>"), endwin(), 0;

	ImageLoader *loader = ImageLoader::get_instance(argv[0]);
	vector<vector<vector<uint32_t>>> img;;
	vector<uint32_t> delay;
	tie(delay, img) = loader->load(argv[1], cols/2, lines);
	vector<vector<string>> asc = IMG2ASCII(img).convert_raw();

	clear();
	refresh();

	for (;;) {
		uint32_t fptr = 0;
		for (auto& frm: asc) {
			int rptr = 0, cptr = 0;
			for (auto& row: frm) {
				for (cptr = 0; cptr < row.size(); cptr++) {
					int col = (unsigned char)row[cptr];
					attrset(COLOR_PAIR(col));
					mvprintw(rptr, cptr*2, "##");
				}
				++rptr;
			}
			mvprintw(rptr, 0, "Press q(Q) to quit");
			refresh();
			usleep(10000 * delay[fptr]);
			++fptr;
		}
		char ch = getch();
		if (ch == 'q' || ch == 'Q') break;
	}

	endwin();

	return 0;
}
