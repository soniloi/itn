#ifndef UPCOMPARATORS_H
#define UPCOMPARATORS_H

#include "UserPairCharacteristic.h"

/* Comparator for comparing two User Pairs; the characteristic will have already been decided */
class UserPairCharacteristicComparator{
public:
	bool operator()(UserPairCharacteristic left, UserPairCharacteristic right){
		return left.getCharacteristic() < right.getCharacteristic();
	}
};

#endif
