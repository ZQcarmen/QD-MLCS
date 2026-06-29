#ifndef __HASH_H__
#define __HASH_H__

#include <math.h>
#include <numeric>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>

#include "point.h"

class RSHash{

public:
	RSHash(unsigned int mod = 10000) : mod(mod) { }

	unsigned int operator() (const PointVHT* point) const;

private:
	unsigned int mod;

};

struct PointKeyEqual{
    bool operator() (const PointVHT* point1, const PointVHT* point2) const;
};

#define MapType int

class PointMap{

public:
	using ps_type = std::unordered_map<PointVHT*, MapType, RSHash, PointKeyEqual>;
	using pair_type = std::pair<PointVHT*, MapType>;

    PointMap(unsigned int mod = 10000, float load_factor = 10000);
    ~PointMap();

    PointMap(const PointMap&) = delete;
    PointMap(PointMap&&) = delete;
    PointMap& operator=(const PointMap&) = delete;
    PointMap& operator=(PointMap&&) = delete;

    bool insert(pair_type& data){ return points->insert(data).second; }
    bool insert(PointVHT* p, MapType i){ return points->insert({p, i}).second; }
    bool find(PointVHT* point){ return points->find(point) != points->end(); }
    MapType operator[] (PointVHT* point) { return points->operator[](point); }
    unsigned int size() const { return points->size(); }
    ps_type* get_points() { return points; }
    std::vector<int> buckets_size(bool store = false, std::string path = "./data.txt");

private:
	ps_type* points;

};

#endif