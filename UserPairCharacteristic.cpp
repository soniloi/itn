#include "UserPairCharacteristic.h"

/* Constructor */
UserPairCharacteristic::UserPairCharacteristic(User* f, User* s, float c){
	this->first = f;
	this->second = s;
	this->characteristic = c;
}

User* UserPairCharacteristic::getFirst(){
	return this->first;
}

User* UserPairCharacteristic::getSecond(){
	return this->second;
}

float UserPairCharacteristic::getCharacteristic(){
	return this->characteristic;
}
