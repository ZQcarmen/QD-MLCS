#ifndef __LCSCAL_INCLUDE_UTILS_H__
#define __LCSCAL_INCLUDE_UTILS_H__

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <utility>
#include <queue>
#include <algorithm>
#include "point.h"

// return a map for a alphabet set and index, such as ('A', 1).
std::unordered_map<char, int> build_alphabet_map(std::string& alphabets);

// return a successor table or tables
using st_type = std::vector< std::vector<int> >;
using sts_type = std::vector< std::vector< std::vector<int> > >;
st_type cal_suc_tab(std::string& seq, std::unordered_map<char, int>& cmap, int len,
	bool disp = false);
sts_type cal_suc_tabs(std::vector<std::string>& seqs, 
	std::unordered_map<char, int>& cmap, int len = -1, bool disp = false);

PointVHT* successor(PointVHT* p, sts_type& SucTabs, int i);

using ct_type = std::vector<std::vector<std::vector<short>>>;
void cal_ct_tabs(std::vector<std::string>& seqs, ct_type& CountTab, std::unordered_map<char, int>& cmap, int len);
int cal_upper1(int* coor, ct_type& DpTabs);

using dpt_type = std::unordered_map<int, std::vector<std::vector<short>>>;
void cal_dp_tabs(std::vector<std::string>& seqs, dpt_type& DpTab, int len);
int cal_upper2(int* coor, dpt_type& DpTabs, int n);

void cal_score_tabs(std::vector<std::string>& seqs, std::vector<std::vector<int>>& ScoreTab, int len, std::unordered_map<char, int>& cmap);

//using scoret_type = std::unordered_map<std::pair<int, int>, std::vector<std::vector<short>>>;
void cal_idxs(std::vector<std::string>& seqs, std::vector<std::pair<int, int>>& idxs, int len, std::unordered_map<char, int>& cmap, std::vector<std::vector<int>>& ScoreTab);
int cal_upper3(int* coor, std::vector<std::pair<int, int>>& idxs, dpt_type& DpTab, int n);
int cal_upper4(int* coor, dpt_type& DpTab, int n);

using dpt3_type = std::pair<std::vector<int>, std::vector<std::vector<std::vector<short>>>>;
void cal_dp3_tabs(std::vector<std::string>& seqs, dpt3_type& Dp3Tab, std::vector<std::vector<int>>& ScoreTab, int len);
int cal_upper_by_dpt3(int* coor, dpt3_type& DpTabs);

float diversity(int* coor, int idx1, int idx2, ct_type& CountTab, int len);
int cal_upperBIG(int* coor, dpt_type& DpTabs, ct_type& CountTab, int n, int len);

#endif