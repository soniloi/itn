/* String tokeniser class */

#ifndef TOKENISER_H
#define TOKENISER_H

#include <string>
#include <vector>
#include <utility>

typedef std::pair<std::vector<std::string>, std::vector<std::string> > stringvectorpair;

class Tokeniser{
public:
	static std::vector<std::string> split(std::string str, char delim);
	static stringvectorpair doubletokenise(std::string input, std::string punc);
};

#endif
