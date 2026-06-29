#include "hash.h"

unsigned int RSHash::operator() (const PointVHT* point) const{
    unsigned int b = 378551 ;
    unsigned int a = 63689 ;
    unsigned int hash = 0 ;
    for(int i = 0; i < point->size; i++){
        hash = hash * a + point->coor[i];
        a *= b;
    }

    return hash % mod;/*
    return std::accumulate(point->coor, point->coor + point->size, 0u, [&](size_t acc, int num) {
                return (acc << 1) ^ std::hash<int>{}(num);
            });*/
}

bool PointKeyEqual::operator() (const PointVHT* point1, const PointVHT* point2) const{
    for(int i = 0; i < point1->size; i++){
        if(point1->coor[i] != point2->coor[i]) return false;
    }
    return true;
}

PointMap::PointMap(unsigned int mod, float load_factor){
	points = new ps_type(mod, RSHash(mod), PointKeyEqual());
	points->max_load_factor(load_factor); // Make sure rehash doesn't occur.
}

std::vector<int> PointMap::buckets_size(bool store, std::string path){
    std::vector<int> ret(points->bucket_count());

    for(unsigned int i = 0; i < points->bucket_count(); i++){
    	ret[i] = points->bucket_size(i);
    }

    if(store){
    	std::ofstream ofs(path);
        for(auto val : ret){
        	ofs << val << std::endl;
        }
    	ofs.close();
    }

    return ret;
}

PointMap::~PointMap(){
    for(auto& p : *points){
        delete p.first;
    }
    delete points;
}