#include "upper.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace {

std::pair<int, int> cal_n_range(int m) {
    if(m <= 10) {
        int n = static_cast<int>(std::ceil(m * 0.5));
        return {n, n};
    }
    if(m <= 100) {
        return {
            static_cast<int>(std::ceil(m * 0.15)),
            static_cast<int>(std::ceil(m * 0.5))
        };
    }
    if(m <= 1000) {
        return {
            static_cast<int>(std::ceil(m * 0.01)),
            static_cast<int>(std::ceil(m * 0.15))
        };
    }
    return {5, 9};
}

int suffix_length(const std::string& seq, int start) {
    start = std::max(0, std::min(start, static_cast<int>(seq.length())));
    return static_cast<int>(seq.length()) - start;
}

int cal_lcs_between_suffixes(const std::string& seq1, int start1, const std::string& seq2, int start2) {
    start1 = std::max(0, std::min(start1, static_cast<int>(seq1.length())));
    start2 = std::max(0, std::min(start2, static_cast<int>(seq2.length())));

    int len1 = static_cast<int>(seq1.length()) - start1;
    int len2 = static_cast<int>(seq2.length()) - start2;
    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));

    for(int i = len1 - 1; i >= 0; --i) {
        for(int j = len2 - 1; j >= 0; --j) {
            if(seq1[start1 + i] == seq2[start2 + j]) {
                dp[i][j] = dp[i + 1][j + 1] + 1;
            }
            else {
                dp[i][j] = std::max(dp[i + 1][j], dp[i][j + 1]);
            }
        }
    }
    return dp[0][0];
}

}


void cal_dp_tabs(std::vector<std::string>& seqs, dps_type& DpTabs, int len)
{
    for(unsigned int i1 = 0; i1 < seqs.size(); i1++) {
        for(unsigned int i2 = i1 + 1; i2 < seqs.size(); i2++) {
            dp_type DpTab(len + 1, std::vector<int>(len + 1, 0));
            for (int x = len - 1; x >= 0; x--) {
                for (int y = len - 1; y >= 0; y--) {
                    if (seqs[i1][x] == seqs[i2][y])
                        DpTab[x][y] = DpTab[x + 1][y + 1] + 1;
                    else
                        DpTab[x][y] = std::max(DpTab[x + 1][y], DpTab[x][y + 1]);
                }
            }
            DpTabs[{i1, i2}] = DpTab;
        }
    }
}

int cal_upper2_by_dpts(PointVHT* p, dps_type& DpTabs, int len)  //step 
{
    int i1 = 0, max_coor1 = p->coor[0];
    for(int i = 1; i < p->size; ++i) {
        if(max_coor1 < p->coor[i]) {
            i1 = i;
            max_coor1 = p->coor[i];
        }
    }
    int upper = INT_MAX;
    int i2 = -1, max_coor2 = -1;
    if(max_coor1 == len) {
        return 0;
    }
    for(int i = 0; i < i1; ++i) {
        if(max_coor1 - p->coor[i] <= 3) {
            upper = std::min(upper, DpTabs[{i, i1}][p->coor[i]][p->coor[i1]]);
        }
        if(max_coor2 < p->coor[i]) {
            i2 = i;
            max_coor2 = std::max(max_coor2, p->coor[i]);
        }
    }
    for(int i = i1 + 1; i < p->size; ++i) {
        if(max_coor1 - p->coor[i] <= 3) {
            upper = std::min(upper, DpTabs[{i1, i}][p->coor[i1]][p->coor[i]]);
        }
        if(max_coor2 < p->coor[i]) {
            i2 = i;
            max_coor2 = std::max(max_coor2, p->coor[i]);
        }
    }
    if(upper < INT_MAX) {
        return upper;
    }
    if(i1 < i2) {
        return DpTabs[{i1, i2}][p->coor[i1]][p->coor[i2]];
    }
    return DpTabs[{i2, i1}][p->coor[i2]][p->coor[i1]];
}

int cal_upper_by_sorted_suffix_subset(PointVHT* p, const std::vector<std::string>& seqs, int level)
{
    int m = p->size;
    if(m <= 0) {
        return level;
    }

    std::vector<int> order(m);
    std::iota(order.begin(), order.end(), 0);
    std::sort(order.begin(), order.end(), [&](int lhs, int rhs) {
        int lhs_len = suffix_length(seqs[lhs], p->coor[lhs]);
        int rhs_len = suffix_length(seqs[rhs], p->coor[rhs]);
        if(lhs_len != rhs_len) {
            return lhs_len < rhs_len;
        }
        return lhs < rhs;
    });

    auto [min_n, max_n] = cal_n_range(m);
    min_n = std::max(1, std::min(min_n, m));
    max_n = std::max(min_n, std::min(max_n, m));

    int best_suffix_upper = INT_MAX;
    int anchor = order[0];

    for(int n = min_n; n <= max_n; ++n) {
        int suffix_upper = suffix_length(seqs[anchor], p->coor[anchor]);
        for(int i = 1; i < n; ++i) {
            int other = order[i];
            int lcs_len = cal_lcs_between_suffixes(
                seqs[anchor], p->coor[anchor],
                seqs[other], p->coor[other]);
            suffix_upper = std::min(suffix_upper, lcs_len);
            if(suffix_upper == 0) {
                break;
            }
        }
        best_suffix_upper = std::min(best_suffix_upper, suffix_upper);
    }

    if(best_suffix_upper == INT_MAX) {
        return level;
    }
    return level + best_suffix_upper;
}
