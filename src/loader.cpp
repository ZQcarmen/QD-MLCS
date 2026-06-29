#include "loader.h"

#include <stdexcept>

static void trim(std::string &s) {
    int first = -1;
    int last = -1;

    if(s.length() == 0) return;
	
    for(unsigned int i = 0; i < s.length(); i++){
		if(s[i] > 32){
			first = i;
			break;
		}
	}
	for(unsigned int i = s.length() - 1; i >= 0; i--){
		if(s[i] > 32){
			last = i;
			break;
		}
	}
	if(last >= first){
		s = s.substr(first, last - first + 1);
	}
	else{
        s.clear();
	}
} 

DataLoader::DataLoader(std::string filepath){
	std::ifstream ifs(filepath);
    if(!ifs.is_open()) {
        throw std::runtime_error("failed to open input file: " + filepath);
    }
    std::vector<std::string> seqs_t;
    bool seq_flag = false;

	while(!ifs.eof()){
        std::string line;
        getline(ifs, line);
        trim(line);
        if(line.length() == 0)
        	continue;
        else if(line[0] == '#')
        	continue;
        else if(line == "sequences%start"){
        	seq_flag = true;
        }
        else if(line == "%end"){
        	seq_flag = false;
        	_seqs = std::move(seqs_t);
        }
        else if(seq_flag){
        	seqs_t.push_back(line);
        }
        else{
        	std::stringstream ss{line};
        	std::string name, val, delimiter;
        	ss >> name >> delimiter >> val;
        	if(name == "alphabets"){
                _alphabets = val;
        	}
        }
	}
}


DataLoader::~DataLoader() {

}
