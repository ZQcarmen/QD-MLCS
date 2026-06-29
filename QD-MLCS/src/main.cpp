#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <queue>
#include <stdexcept>
#include <set>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

#include "hash.h"
#include "loader.h"
#include "point.h"
#include "upper.h"
#include "utils.h"

#if defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#include <sys/time.h>
#endif

using namespace std;

static bool useMethod1 = true;
static bool useMethod2 = true;
static bool enableBacktrack = false;
static bool enableAllMlcs = false;

static int alphanum;
static int seqnum;
static int Length = 0;
static vector<PointMap*> VHT;
vector<PointGHT*> GHT;
static vector<vector<vector<int>>> SucTabs;
static vector<MissPoint*> missPoints;

static dps_type DpTabs;

static PointGHT* endPoint = new PointGHT('#', 0);
static vector<pair<PointVHT*, int>> last;
static int Index = 0;
static int mlcs_length = 0;
static vector<vector<pair<int, int*>>> minSumPoints;
static unordered_map<char, int> cmap;
static int del_num = 0;

class cmp {
public:
    bool operator() (const pair<PointVHT*, int>& p1, const pair<PointVHT*, int>& p2) const {
        return p1.second < p2.second;
    }
};

static int lower;
static int theta = 2;
static priority_queue<pair<PointVHT*, int>, vector<pair<PointVHT*, int>>, cmp> pq;

using sequence_set = set<string>;

static double peak_rss_mb() {
#if defined(__unix__) || defined(__APPLE__)
    rusage rs {};
    getrusage(RUSAGE_SELF, &rs);
    return rs.ru_maxrss / 1024.0;
#else
    return 0.0;
#endif
}

int calMin(PointVHT* p) {
    int min_val = p->coor[0];
    for(int i = 1; i < p->size; i++) {
        min_val = min(min_val, p->coor[i]);
    }
    return min_val;
}

int calMinIndex(PointVHT* p) {
    int min_val = p->coor[0], min_index = 0;
    for(int i = 1; i < p->size; i++) {
        if(p->coor[i] < min_val) {
            min_val = p->coor[i];
            min_index = i;
        }
    }
    return min_index;
}

int calScore(PointVHT* p) {
    int sum = p->coor[0], max_val = p->coor[0];
    for(int i = 1; i < p->size; i++) {
        sum += p->coor[i];
        max_val = max(max_val, p->coor[i]);
    }
    return sum * 9 + max_val * seqnum;
}

char alpha_from_point(PointVHT* p, string& seq, int lev) {
    if(lev == 0 || p->coor[0] <= 0) {
        return '#';
    }
    return seq[p->coor[0] - 1];
}

int pick_smallest_node(const unordered_set<int>& nodes, int expected_level) {
    int best = INT_MAX;
    for(int idx : nodes) {
        if(idx >= 0 && idx < static_cast<int>(GHT.size()) && GHT[idx] != nullptr && GHT[idx]->level == expected_level) {
            best = min(best, idx);
        }
    }
    return best == INT_MAX ? -1 : best;
}

int pick_predecessor(int idx) {
    PointGHT* node = GHT[idx];
    int expected_level = node->level - 1;
    int best = INT_MAX;
    for(int pre : node->pres) {
        if(pre >= 0 && pre < static_cast<int>(GHT.size()) && GHT[pre] != nullptr && GHT[pre]->level == expected_level) {
            best = min(best, pre);
        }
    }
    return best == INT_MAX ? -1 : best;
}

vector<int> backtrack_path() {
    vector<int> path;
    if(endPoint->level <= 0) {
        return path;
    }

    int idx = pick_smallest_node(endPoint->pres, endPoint->level);
    while(idx >= 0) {
        PointGHT* node = GHT[idx];
        if(node == nullptr) {
            path.clear();
            return path;
        }
        if(node->level == 0) {
            break;
        }
        path.push_back(idx);
        idx = pick_predecessor(idx);
    }

    reverse(path.begin(), path.end());
    return path;
}

void print_backtrack_result() {
    vector<int> path = backtrack_path();
    if(endPoint->level > 0 && static_cast<int>(path.size()) != endPoint->level) {
        cout << "mlcs sequence : <backtrack failed>" << endl;
        return;
    }

    string sequence;
    sequence.reserve(path.size());
    for(int idx : path) {
        sequence.push_back(GHT[idx]->alpha);
    }

    cout << "mlcs sequence : " << sequence << endl;
    cout << "trace path : start";
    for(int idx : path) {
        cout << " -> " << idx;
    }
    cout << " -> end" << endl;
}

const sequence_set& collect_sequences_to_node(int idx, unordered_map<int, sequence_set>& memo) {
    auto it = memo.find(idx);
    if(it != memo.end()) {
        return it->second;
    }

    sequence_set results;
    PointGHT* node = GHT[idx];
    if(node == nullptr) {
        auto inserted = memo.emplace(idx, sequence_set());
        return inserted.first->second;
    }

    if(node->level == 0) {
        results.insert("");
    }
    else {
        for(int pre : node->pres) {
            if(pre < 0 || pre >= static_cast<int>(GHT.size()) || GHT[pre] == nullptr) {
                continue;
            }
            if(GHT[pre]->level != node->level - 1) {
                continue;
            }
            const sequence_set& prefixes = collect_sequences_to_node(pre, memo);
            for(const string& prefix : prefixes) {
                results.insert(prefix + node->alpha);
            }
        }
    }

    auto inserted = memo.emplace(idx, move(results));
    return inserted.first->second;
}

sequence_set collect_all_mlcs_sequences() {
    sequence_set sequences;
    unordered_map<int, sequence_set> memo;

    if(endPoint->level == 0) {
        sequences.insert("");
        return sequences;
    }

    for(int idx : endPoint->pres) {
        if(idx < 0 || idx >= static_cast<int>(GHT.size()) || GHT[idx] == nullptr) {
            continue;
        }
        if(GHT[idx]->level != endPoint->level) {
            continue;
        }
        const sequence_set& node_sequences = collect_sequences_to_node(idx, memo);
        sequences.insert(node_sequences.begin(), node_sequences.end());
    }

    return sequences;
}

void print_all_mlcs_sequences() {
    sequence_set sequences = collect_all_mlcs_sequences();
    cout << "mlcs count : " << sequences.size() << endl;
    cout << "mlcs sequences:" << endl;
    for(const string& sequence : sequences) {
        if(sequence.empty()) {
            cout << "<empty>" << endl;
        }
        else {
            cout << sequence << endl;
        }
    }
}

bool parse_args(int argc, char* argv[], string& input_path) {
    for(int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if(arg == "--trace" || arg == "--backtrack" || arg == "-t") {
            enableBacktrack = true;
        }
        else if(arg == "--all-mlcs" || arg == "--all" || arg == "-a") {
            enableAllMlcs = true;
        }
        else if(!arg.empty() && arg[0] == '-') {
            return false;
        }
        else if(input_path.empty()) {
            input_path = arg;
        }
        else {
            return false;
        }
    }
    return !input_path.empty();
}

void pick(vector<PointVHT*>& E) {
    priority_queue<pair<PointVHT*, int>, vector<pair<PointVHT*, int>>, cmp>().swap(pq);
    for(PointVHT* p : E) {
        int ps = calScore(p);
        if(pq.size() < theta) {
            pq.push({p, ps});
        }
        else if(pq.top().second > ps) {
            delete pq.top().first;
            pq.pop();
            pq.push({p, ps});
        }
        else {
            delete p;
        }
    }
}

int calLower(PointVHT* start, int lev) {
    PointVHT* p = new PointVHT(start);
    pq.push({p, 0});
    while(!pq.empty()){
        vector<PointVHT*> E;
        while(!pq.empty()) {
            for(int i = 0; i < alphanum; i++){
                PointVHT* suc = successor(pq.top().first, SucTabs, i);
                if(suc) {
                    E.push_back(suc);
                }
            }
            delete pq.top().first;
            pq.pop();
        }
        pick(E);
        lev++;
    }

    return lev - 1;
}

void addPointToVHT(int pre_idx, PointVHT* p, string& seq, int lev) {
    int id = calMin(p);
    while (VHT.size() <= id) {
        VHT.push_back(new PointMap(100000, 5));
    }
    if(VHT[id] == nullptr) {
        VHT[id] = new PointMap(100000, 5);
    }
    if(VHT[id]->find(p)) {
        int old_idx = (*VHT[id])[p];
        if(GHT[old_idx]->level < lev) {
            GHT[old_idx]->pres.clear();
            GHT[old_idx]->pres.insert(pre_idx);
            GHT[old_idx]->level = lev;
        }
        else if(GHT[old_idx]->level == lev) {
            GHT[old_idx]->pres.insert(pre_idx);
        }
        delete p;
    }
    else {
        VHT[id]->insert(p, Index);
        GHT.emplace_back(new PointGHT(alpha_from_point(p, seq, lev), lev));
        GHT[Index]->pres.insert(pre_idx);
        Index++;
    }
}

void connectToEnd(int idx, PointVHT* p, int lev) {
    if(endPoint->level < lev) {
        last.clear();
        last.push_back({new PointVHT(p), lev});
        endPoint->pres.clear();
        endPoint->pres.insert(idx);
        endPoint->level = lev;
    }
    else if(endPoint->level == lev) {
        last.push_back({new PointVHT(p), lev});
        endPoint->pres.insert(idx);
    }
}

void calMissPoint(string& seq) {
    vector<MissPoint*> nextMissPoints;
    for(auto mpoint : missPoints) {
        PointGHT* p_ght = GHT[mpoint->idx];
        int upper = cal_upper2_by_dpts(mpoint->p, DpTabs, Length) + p_ght->level;
        if(upper > lower) {
            for(int i = 0; i < alphanum; i++) {
                if(p_ght->isMiss(i)) {
                    PointVHT* suc = successor(mpoint->p, SucTabs, i);
                    if(suc) {
                        p_ght->addSuc(i);
                        addPointToVHT(mpoint->idx, suc, seq, p_ght->level + 1);
                    }
                }
            }
        }
        if(p_ght->isAllMiss()) {
            connectToEnd(mpoint->idx, mpoint->p, p_ght->level);
        }
        if(p_ght->hasMiss()) {
            nextMissPoints.push_back(mpoint);
        }
        else {
            delete mpoint;
        }
    }
    missPoints.clear();
    missPoints = nextMissPoints;
}

void constructGraph(string& seq, int num) {
    if(num == 1) {
        PointVHT* startPoint = new PointVHT(0);
        addPointToVHT(-1, startPoint, seq, 0);
    }
    else {
        calMissPoint(seq);
    }
    int vht_id = 0;
    while(vht_id < VHT.size()) {
        if(VHT[vht_id] == nullptr) {
            vht_id++;
            continue;
        }
        for(auto& [p, idx] : *(VHT[vht_id]->get_points())) {
            int upper = cal_upper2_by_dpts(p, DpTabs, Length) + GHT[idx]->level;
            if(upper >= lower) {
                for(int i = 0; i < alphanum; i++) {
                    PointVHT* suc = successor(p, SucTabs, i);
                    if(suc) {
                        GHT[idx]->addSuc(i);
                        addPointToVHT(idx, suc, seq, GHT[idx]->level + 1);
                    }
                }
            }
            if(GHT[idx]->isAllMiss()) {
                connectToEnd(idx, p, GHT[idx]->level);
            }
            if(GHT[idx]->hasMiss()) {
                missPoints.emplace_back(new MissPoint(p, idx));
            }
        }
        delete VHT[vht_id];
        VHT[vht_id] = nullptr;
        vht_id++;
    }
}

int main(int argc, char* argv[]) {
    DataLoader *ds = nullptr;
    string input_path;

    if(parse_args(argc, argv, input_path)){
        try {
            ds = new DataLoader(input_path);
        }
        catch(const std::exception& ex) {
            cerr << ex.what() << endl;
            return -1;
        }
    }
    else{
        cout << "usage: " << argv[0] << " [--trace|--backtrack|-t] [--all-mlcs|--all|-a] <input.seq>" << endl;
        return -1;
    }
    alphanum = ds->alphabets().size();
    seqnum = ds->data().size();
    PointVHT::size = seqnum;
    cmap = build_alphabet_map(ds->alphabets());
    last.push_back({new PointVHT(0), 0});

    int incr_lenghth = 0;
    double total_time = 0;
    int cnt = 0;
    double rs = 0;

    while(1) {
        cout << '[' << cnt <<  "] Please input increased Length(Enter 0 to exit): ";

        cin >> incr_lenghth;
        if(incr_lenghth == 0) break;
        Length += incr_lenghth;
        SucTabs.clear();
        SucTabs = cal_suc_tabs(ds->data(), cmap, Length);
        cal_dp_tabs(ds->data(), DpTabs, Length);
        auto start_t = chrono::system_clock::now();
        double rs1 = peak_rss_mb();
        for(auto& [p, lev] : last) {
            lower = max(lower, calLower(p, lev));
            delete p;
        }
        last.clear();
        constructGraph(ds->data()[0], cnt + 1);

        auto end_t = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_t - start_t);
        total_time += double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den;
        double rs2 = peak_rss_mb();
        rs += max(0.0, rs2 - rs1);
        printf("memory resource: %.1lf MB\n", rs);
        cout << "miss point num: " << missPoints.size() << endl;
        cout << "GHT point num: " << Index << endl;
        cout << "Delete num: " << del_num << endl;
        cout << "lcs length : " << endPoint->level << endl;
        if(enableBacktrack) {
            print_backtrack_result();
        }
        if(enableAllMlcs) {
            print_all_mlcs_sequences();
        }
        cout << "time(sec) : " << double(duration.count()) * chrono::microseconds::period::num / chrono::microseconds::period::den << endl;
        cout << "total time(sec) : " << total_time << endl << endl;
        cnt++;
    }
    cout << endl << "total time(sec) : " << total_time << endl;
    return 0;
}
