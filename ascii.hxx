#ifndef __ASCII__
#define __ASCII__
#include"xterm.h"
#include"metric.hxx"
#include<vector>
#include<string>
#include<sstream>
#include<cstdint>
using std::vector;
using std::string;
using std::ostringstream;

#include<cstdio>
/* Converter Class */
class IMG2ASCII {
private:
	vector<vector<vector<uint32_t>>> rgbimage;
	ColorMetric *metric;

public:
	~IMG2ASCII() { delete metric; }

	explicit IMG2ASCII(const vector<vector<vector<uint32_t>>>& _rgbimage):
		rgbimage(_rgbimage), metric(new RGBL1Metric) {}

	explicit IMG2ASCII(const vector<vector<vector<uint32_t>>>& _rgbimage,
			  ColorMetric *_metric):
		rgbimage(_rgbimage), metric(_metric) {}

	vector<vector<string>> convert(const string ch = "##")
	{
		if (rgbimage.size() == 0) {
			return {{""}};
		}

		vector<vector<string>> ret(rgbimage.size());
		uint32_t frm = 0;
		for (auto& frame: rgbimage) {
			for (auto& row: frame) {
				ostringstream line;
				for (auto& pix: row)
					line << "[38;5;" << (metric->find_closest(pix, xterm)) << "m" << ch;
				ret[frm].push_back(line.str());
			}
			++frm;
		}

		return ret;
	}

	vector<vector<string>> convert_raw()
	{
		if (rgbimage.size() == 0) {
			return {{""}};
		}

		vector<vector<string>> ret(rgbimage.size());
		uint32_t frm = 0;
		for (auto& frame: rgbimage) {
			uint32_t line = 0;
			ret[frm].resize(frame.size());
			for (auto& row: frame) {
				ret[frm][line].resize(row.size());
				uint32_t col = 0;
				for (auto& pix: row) {
					ret[frm][line][col] = metric->find_closest(pix, xterm);
					++col;
				}
				++line;
			}
			++frm;
		}

		return ret;
	}

};
#endif
