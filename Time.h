#ifndef TIME_H
#define TIME_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include "Config.h"
#include "Definitions.h"

class Time{
private:
	static const int daycounts[];
	
	// FIXME: Consider removing the next three and representing time internally as seconds only
	int year;
	int month;
	int day;
	long long seconds;
	inline bool isLeapYear(int y);

public:
	Time(int y=YEAR_ZERO, int mo=0, int d=0, int h=0, int mi=0, int s=0);
	long long getTimeInSeconds();
	int getYear();
	int getMonth();
	int getDay();
	bool operator ==(Time that) const;
	bool operator !=(Time that) const;
	bool operator >(Time that) const;
	bool operator <(Time that) const;
	bool operator >=(Time that) const;
	bool operator <=(Time that) const;
	long long getDifferenceInSeconds(const Time& that) const;
	std::string display();
};

#endif
