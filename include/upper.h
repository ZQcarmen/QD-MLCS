#ifndef __LCSCAL_BIGMLCS_UPPER2_H__
#define __LCSCAL_BIGMLCS_UPPER2_H__

#include <vector>
#include <unordered_map>
#include <string>
#include <limits.h>

#include "point.h"

class PairHash{

public:
	PairHash() { }

	unsigned int operator() (const std::pair<int, int>& p) const {
		return p.first * 10001 + p.second;
	}

private:

};

struct PairEqual{
    bool operator() (const std::pair<int, int>& p1, const std::pair<int, int>& p2) const {
		return (p1.first == p2.first) && (p1.second == p2.second);
	}
};

using dp_type = std::vector<std::vector<int>>;
using dps_type = std::unordered_map<std::pair<int, int>, dp_type, PairHash, PairEqual>;
void cal_dp_tabs(std::vector<std::string>& seqs, dps_type& DpTabs, int len);
int cal_upper1_by_dpts(PointVHT* coor, dps_type& DpTabs);
int cal_upper2_by_dpts(PointVHT* coor, dps_type& DpTabs, int len);
int cal_upper_by_sorted_suffix_subset(PointVHT* coor, const std::vector<std::string>& seqs, int level);

#endif
