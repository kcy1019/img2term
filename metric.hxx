#ifndef __METRIC__
#define __METRIC__
#include<cstdint>
#include<vector>
#include<algorithm>
using std::abs;
using std::vector;

/* Metric Abstract Class */
class ColorMetric {
public:
	virtual inline long long distance(uint32_t lhs, uint32_t rhs) const = 0;
	virtual inline uint32_t find_closest(uint32_t target,
								   const vector<uint32_t>& candidates) const = 0;
};

/* L1-Metric for RGB Space */
class RGBL1Metric: public ColorMetric {
public:
	virtual inline long long distance(const uint32_t lhs, const uint32_t rhs) const {
		unsigned int lr = (lhs & 0xFF0000);
		unsigned int lg = (lhs & 0x00FF00);
		unsigned int lb = (lhs & 0x0000FF);
		unsigned int rr = (rhs & 0xFF0000);
		unsigned int rg = (rhs & 0x00FF00);
		unsigned int rb = (rhs & 0x0000FF);
		return abs(lr - rr) + abs(lg - rg) + abs(lb - rb);
	}

	virtual inline uint32_t find_closest(uint32_t target,
								   const vector<uint32_t>& candidates) const {
		int argmin = 0;
		long long min_distance = 1LL << 60;
		for (int ptr = 0; ptr < candidates.size(); ptr++) {
			const uint32_t& cand = candidates[ptr];
			long long dist = distance(target, cand);
			if (dist < min_distance) {
				min_distance = dist;
				argmin = ptr;
			}
		}
		return argmin;
	}
};

#endif
