#ifndef __POINT_H__
#define __POINT_H__
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>

struct PointVHT {
    int* coor;
    static int size;

    PointVHT(): coor(new int[size]){}
    PointVHT(int val);
    PointVHT(PointVHT* p): coor(new int[size]) {
        for(int i = 0; i < size; ++i) {
            coor[i] = p->coor[i];
        }
    }

    void print(std::string flag);
    ~PointVHT();
};

struct PointGHT {
    char alpha;
    std::unordered_set<int> pres;
    int level;
    int sucs; // A C G T;

    PointGHT(char alp, int lev): alpha(alp), level(lev), sucs(0) {}

    void addSuc(int suc); //A:0 B:1 C:2 D:3
    bool hasSuc(int suc);
    bool isMiss(int i) {return (sucs & (1 << i)) == 0; }
    bool hasMiss() {return sucs != 15; }
    bool isAllMiss() {return sucs == 0; }

    void print() {
        std::cout << level << std::endl;
    }
};

struct MissPoint {
    PointVHT* p;
    int idx;

    MissPoint(PointVHT* vhtp, int idx):
        p(new PointVHT(vhtp)), idx(idx) {}
    ~MissPoint() {
        if(p) {
            delete p;
        }
    }
};

#endif