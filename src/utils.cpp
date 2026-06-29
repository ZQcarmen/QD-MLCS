#include "utils.h"

#include <cstdint>

std::unordered_map<char, int> build_alphabet_map(std::string& alphabets) {
    std::unordered_map<char, int> cmap;
    for(unsigned int i = 0; i < alphabets.length(); i++){
        cmap.insert({alphabets[i], i});
    }
    return cmap;
}

st_type cal_suc_tab(std::string& seq, std::unordered_map<char, int>& cmap, int len = -1, bool disp) {
    if(len == -1) {
        len = seq.length();
    }
    st_type SucTab(cmap.size(), std::vector<int>(len + 1));

    // calculate successor table
    for (unsigned int i = 0; i < cmap.size(); i++) {
        SucTab[i][len] = -1;
    }

    for (int j = len - 1; j >= 0; j--) {
        for (unsigned int i = 0; i < cmap.size(); i++) {
            SucTab[i][j] = SucTab[i][j + 1];
        }
        SucTab[cmap[seq[j]]][j] = j + 1;
    }

    if(disp){
        std::cout << "\n  |         ";
        for(int i = 0; i < len; i++) std::cout << seq[i] << "    ";
        std::cout << std::endl;
        for(int i = 0; i < len + 2; i++) std::cout << "----" << "    ";
        std::cout << std::endl;
        for (auto m : cmap) {
            std::cout << m.first << " |     ";
            for (int j = 0; j <= len; j++) {
                std::cout << SucTab[m.second][j] << "    ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    return SucTab;
}

sts_type cal_suc_tabs(std::vector<std::string>& seqs, 
	std::unordered_map<char, int>& cmap, int len, 
	bool disp)
{    
    sts_type SucTabs;
    for(auto& seq : seqs){
        SucTabs.push_back(cal_suc_tab(seq, cmap, len, disp));
    }
    return SucTabs;
    
}


PointVHT* successor(PointVHT* p, sts_type& SucTabs, int i) {
    PointVHT* suc = new PointVHT();

    for(unsigned int j = 0; j < SucTabs.size(); ++j){
        suc->coor[j] = SucTabs[j][i][p->coor[j]];
        if(suc->coor[j] < 0){ // dominants do not exists.
            delete suc;
            return nullptr;
        }
    }

    return suc;
}

void cal_ct_tabs(std::vector<std::string>& seqs, ct_type& CountTab, std::unordered_map<char, int>& cmap, int len) {
    int n = seqs.size();
    CountTab.clear();
    CountTab.resize(n, std::vector<std::vector<short>>(cmap.size(), std::vector<short>(len + 1)));
    for(int i = 0; i < n; ++i) {
        for (unsigned int j = 0; j < cmap.size(); ++j) {
            CountTab[i][j][len] = 0;
        }

        for (int j = len - 1; j >= 0; --j) {
            for (unsigned int k = 0; k < cmap.size(); ++k) {
                CountTab[i][k][j] = CountTab[i][k][j + 1];
            }
            CountTab[i][cmap[seqs[i][j]]][j]++;
        }
    }
}
int cal_upper1(int* coor, ct_type& CountTab) {
    int upper = 0;

    for(unsigned int i = 0; i < CountTab[0].size(); ++i){
        short minval = INT16_MAX;
        for(unsigned int j = 0; j < CountTab.size(); j++){
            minval = std::min(CountTab[j][i][coor[j]], minval);
        }
        upper += minval;
    }
    return upper;
}

void cal_dp_tabs(std::vector<std::string>& seqs, dpt_type& DpTab, int len) {
    int n = seqs.size();
    DpTab.clear();
    for(unsigned int i1 = 0; i1 < seqs.size(); i1++) {
        for(unsigned int i2 = i1 + 1; i2 < seqs.size(); i2++) {
            std::vector<std::vector<short>> tab(len + 1, std::vector<short>(len + 1, 0));
            for (int x = len - 1; x >= 0; x--) {
                for (int y = len - 1; y >= 0; y--) {
                    if (seqs[i1][x] == seqs[i2][y])
                        tab[x][y] = tab[x + 1][y + 1] + 1;
                    else
                        tab[x][y] = std::max(tab[x + 1][y], tab[x][y + 1]);
                }
            }
            DpTab[i1 * n + i2] = tab;
        }
    }
}
int cal_upper2(int* coor, dpt_type& DpTab, int n) {
    short upper = INT16_MAX;
    for(int i = 1; i < n; ++i) {
        upper = std::min(upper, DpTab[(i - 1) * n + i][coor[i - 1]][coor[i]]);
    }
    return upper;
}

class score_cmp
{
public:
    bool operator() (const std::vector<int>& p1, const std::vector<int>& p2) const
    {
        return p1[0] < p2[0];
    }
};

void cal_score_tabs(std::vector<std::string>& seqs, std::vector<std::vector<int>>& ScoreTab, int len, std::unordered_map<char, int>& cmap) {
    int n = seqs.size();
    ScoreTab.clear();
    std::vector<std::vector<int>> cnt(n, std::vector<int>(cmap.size(), 0));
    for(int i = 0; i < n; ++i) {
        for(int j = 0; j < len; j++) {
            cnt[i][cmap[seqs[i][j]]]++;
        }
    }
    for(int i = 0; i < n; ++i) {
        for(int j = i + 1; j < n; ++j) {
            int score = 0;
            for(int k = 0; k < cmap.size(); ++k) {
                score += std::min(cnt[i][k], cnt[j][k]);
            }
            ScoreTab.push_back({i, j, score});
        }
    }
    std::sort(ScoreTab.begin(), ScoreTab.end(), 
        [](const std::vector<int>& p1, const std::vector<int>& p2) {
        return p1[2] < p2[2]; });
}

void cal_idxs(std::vector<std::string>& seqs, std::vector<std::pair<int, int>>& idxs, int len, std::unordered_map<char, int>& cmap, std::vector<std::vector<int>>& ScoreTab) {
    int cal_num = 6;
    int n = seqs.size();
    idxs.clear();
    for(int i = 0; i < cal_num; ++i) {
        idxs.push_back({ScoreTab[i][0], ScoreTab[i][1]});
    }
}
int cal_upper3(int* coor, std::vector<std::pair<int, int>>& idxs, dpt_type& DpTab, int n) {
    short upper = INT16_MAX;
    for(auto& id : idxs) {
        int i1 = id.first;
        int i2 = id.second;
        upper = std::min(upper, DpTab[i1 * n + i2][coor[i1]][coor[i2]]);
    }
    return upper;
}

int cal_upper4(int* coor, dpt_type& DpTab, int n) {
    int max_coor = coor[0], i1 = 0;
    for(int i = 1; i < n; i++) {
        if(coor[i] > max_coor) {
            max_coor = coor[i];
            i1 = i;
        }
    }
    short upper = INT16_MAX;
    for(int i2 = 0; i2 < i1; i2++) {
        upper = std::min(upper, DpTab[i2 * n + i1][coor[i2]][coor[i1]]);
    }
    for(int i2 = i1 + 1; i2 < n; i2++) {
        upper = std::min(upper, DpTab[i1 * n + i2][coor[i1]][coor[i2]]);
    }
    return upper;
}


void cal_dp3_tabs(std::vector<std::string>& seqs, dpt3_type& Dp3Tab, std::vector<std::vector<int>>& ScoreTab, int len) {
    int idx1 = ScoreTab[0][0];
    int idx2 = ScoreTab[0][1];
    int n = ScoreTab.size();
    std::vector<int> c(seqs.size(), 0);
    for(int i = 1; i < n; ++i) {
        if(ScoreTab[i][0] == idx1 || ScoreTab[i][0] == idx2) {
            c[ScoreTab[i][1]] += ScoreTab[i][2];
        }
        else if(ScoreTab[i][1] == idx1 || ScoreTab[i][1] == idx2) {
            c[ScoreTab[i][0]] += ScoreTab[i][2];
        }
    }

    int idx3 = -1, min_c = INT32_MAX;
    for(int i = 0; i < seqs.size(); ++i) {
        if(i != idx1 && i != idx2) {
            if(min_c > c[i]) {
                min_c = c[i];
                idx3 = i;
            }
        }
    }
    Dp3Tab = {{idx1, idx2, idx3}, std::vector<std::vector<std::vector<short>>>(len + 1, std::vector<std::vector<short>>(len + 1, std::vector<short>(len + 1, 0)))};
    //Dp3Tab = {{0, 1, 2}, std::vector<std::vector<std::vector<short>>>(len + 1, std::vector<std::vector<short>>(len + 1, std::vector<short>(len + 1, 0)))};
    for (int x = len - 1; x >= 0; x--) {
        for (int y = len - 1; y >= 0; y--) {
            for(int z = len - 1; z >= 0; z--) {
                if(seqs[idx1][x] == seqs[idx2][y] && seqs[idx1][x] == seqs[idx3][z])
                    Dp3Tab.second[x][y][z] = Dp3Tab.second[x + 1][y + 1][z + 1] + 1;
                else 
                    Dp3Tab.second[x][y][z] = std::max(Dp3Tab.second[x + 1][y][z], std::max(Dp3Tab.second[x][y + 1][z], Dp3Tab.second[x][y][z + 1]));
            }
        }
    }
}
int cal_upper_by_dpt3(int* coor, dpt3_type& Dp3Tabs) {
    return Dp3Tabs.second[coor[Dp3Tabs.first[0]]][coor[Dp3Tabs.first[1]]][coor[Dp3Tabs.first[2]]];
}


float diversity(int* coor, int idx1, int idx2, ct_type& CountTab, int len) {
    int d = 0;
    for(int i = 0; i < CountTab[0].size(); ++i) {
        d += CountTab[idx1][i][coor[idx1]] * (CountTab[idx1][i][coor[idx1]] - CountTab[idx2][i][coor[idx2]]);
    }
    return (float) d / len;
}
int cal_upperBIG(int* coor, dpt_type& DpTabs, ct_type& CountTab, int n, int len) {
    int idx1 = 0, max_val = coor[0];
    for(int i = 1; i < n; ++i) {
        if(coor[i] > max_val) {
            idx1 = i;
            max_val = coor[i];
        }
    }
    if(max_val == len) return 0;
    float max_diversity = -100000;
    int idx2 = -1;
    for(int i = 0; i < n; ++i) {
        if(i != idx1) {
            float ds = diversity(coor, idx1, i, CountTab, len - max_val);
            if(ds > max_diversity) {
                idx2 = i;
                max_diversity = ds;
            }
        }
    }
    if(idx1 > idx2) {
        std::swap(idx1, idx2);
    }
    return DpTabs[idx1 * n + idx2][coor[idx1]][coor[idx2]];
}
