#ifndef COMPARATORS_H
#define COMPARATORS_H

#include <utility>

/* Comparator for comparing two Pair<string, int> objects by the int value */
class StringIntPairComparator{
public:
	bool operator()(std::pair<std::string, int> left, std::pair<std::string, int> right){
		return left.second < right.second;
	}
};

#endif
