/* Class representing a pair of users with certain interactions */

#ifndef UPC_H
#define UPC_H

#include <string>
#include "User.h"

class UserPairCharacteristic{
private:
	User* first; // The user who has performed more of the relevant action
	User* second; // The user who has performed less (or equal) of the relevant action
	float characteristic; // Usually a percentage of some interaction index

public:
	UserPairCharacteristic(User* f, User* s, float c);
	User* getFirst();
	User* getSecond();
	float getCharacteristic();
};

#endif
