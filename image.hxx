#ifndef __IMAGELOADER__
#define __IMAGELOADER__
#include<Magick++.h>
#include<cstdint>
#include<cmath>
#include<string>
#include<vector>
#include<tuple>
#include<list>
using std::list;
using std::tuple;
using std::vector;
using std::string;
using namespace Magick;

/* Image Loader Singleton Class */
class ImageLoader {
	explicit ImageLoader(char *argv0)
	{
		InitializeMagick(argv0);
	}
public:
	static ImageLoader* get_instance(char *argv0)
	{
		static ImageLoader *inst = nullptr;
		if (inst == nullptr) inst = new ImageLoader(argv0);
		return inst;
	}
	tuple<vector<uint32_t>, vector<vector<vector<uint32_t>>>>
	load(const string path, uint32_t width)
	{
		vector<Image> frames;
		readImages(&frames, path);
		double rows_by_cols = double(frames.begin() -> rows()) /
							  double(frames.begin() -> columns());
		uint32_t height = round(rows_by_cols * width);

		Geometry new_size(width, height);
		new_size.aspect(false);

		vector<vector<vector<uint32_t>>> ret(frames.size());
		vector<uint32_t> delay(frames.size());

		uint32_t frm = 0;
		for (auto& frame: frames) {
			if (frame.columns() > width)
				frame.resize(new_size);
			ret[frm].resize(height);
			for (uint32_t r = 0; r < height; r++) {
				ret[frm][r].resize(width);
				for (uint32_t c = 0; c < width; c++) {
					ColorRGB rgb = frame.pixelColor(c, r);
					uint32_t pix = 0;
					pix |= uint32_t(rgb.red() * 255) << 16;
					pix |= uint32_t(rgb.green() * 255) << 8;
					pix |= uint32_t(rgb.blue() * 255) << 0;
					ret[frm][r][c] = pix;
				}
			}
			delay[frm] = frame.animationDelay();
			++frm;
		}

		return tie(delay, ret);
	}
};

#endif
