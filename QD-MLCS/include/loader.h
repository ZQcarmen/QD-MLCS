#ifndef __LCSCAL_INCLUDE_LOADER_H__
#define __LCSCAL_INCLUDE_LOADER_H__

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

class DataLoader {

public:
    DataLoader(std::string filepath);
    ~DataLoader();

	std::vector<std::string>& data() { return _seqs; }
	std::string& alphabets() { return _alphabets; }
	int size() { return _seqs.size(); }

private:
    std::vector<std::string> _seqs;
    std::string _alphabets;

};

#endif