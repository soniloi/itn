#include "Time.h"

using std::string;
using std::stringstream;

const int Time::daycounts[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* Constructor */
Time::Time(int y, int mo, int d, int h, int mi, int s){

	// FIXME: better bounds checks
	if(mo >= MONTHS_PER_YEAR){
		std::cerr << "Invalid month detected: " << mo << ", exiting" << std::endl;
		exit(1);
	}

	this->year = y;
	this->month = mo;
	this->day = d;

	int daytot = 0;

	for(int i = 0; i < mo; i++)
		daytot += daycounts[i];
	daytot += d;

	for(int i = YEAR_ZERO; i <= y; i++){
		if(isLeapYear(i))
			daytot++;
	}

	long long secs = s;
	secs += mi*SECONDS_PER_MINUTE;
	secs += h*MINUTES_PER_HOUR*SECONDS_PER_MINUTE;
	secs += daytot*HOURS_PER_DAY*MINUTES_PER_HOUR*SECONDS_PER_MINUTE;
	secs += (y-YEAR_ZERO)*DAYS_PER_YEAR*HOURS_PER_DAY*MINUTES_PER_HOUR*SECONDS_PER_MINUTE;

	this->seconds = secs;
}

bool Time::isLeapYear(int y){
	if(y % LEAPYEAR_FREQUENCY)
		return false;
	return ((y % LEAPYEAR_FIRST_CORRECTOR) != 0 || (y % LEAPYEAR_SECOND_CORRECTOR) == 0);
}

/* Return the number of absolute seconds in this time */
long long Time::getTimeInSeconds(){
	return this->seconds;
}

int Time::getYear(){
	return this->year;
}

int Time::getMonth(){
	return this->month;
}

int Time::getDay(){
	return this->day;
}

bool Time::operator ==(Time that) const{
	return this->seconds == that.seconds;
}

bool Time::operator !=(Time that) const{
	return this->seconds != that.seconds;
}

bool Time::operator >(Time that) const{
	return this->seconds > that.seconds;
}

bool Time::operator <(Time that) const{
	return this->seconds < that.seconds;
}

bool Time::operator >=(Time that) const{
	return this->seconds >= that.seconds;
}

bool Time::operator <=(Time that) const{
	return this->seconds < that.seconds;
}

/* Return the difference in seconds between this time and another */
long long Time::getDifferenceInSeconds(const Time& that) const{
	return this->seconds - that.seconds;
}

/* Return a string representation of Gregorian calendar version of this timestamp */
string Time::display(){
	char datesep = Config::getChar(KEY_CONF_OUT_DATE_SEPARATOR);
	char timesep = Config::getChar(KEY_CONF_OUT_TIME_SEPARATOR);
	
	stringstream result;
	int total = this->seconds;
	
	int secs = total % (HOURS_PER_DAY * MINUTES_PER_HOUR * SECONDS_PER_MINUTE);
	total /= (HOURS_PER_DAY * MINUTES_PER_HOUR * SECONDS_PER_MINUTE);
	
	// Calculate year
	int yrs = YEAR_ZERO;
	while(total >= DAYS_PER_YEAR){
		total -= DAYS_PER_YEAR;
		if(total > 0 && isLeapYear(yrs))
			total--;
		yrs++;
	}
	
	// Calculate month
	int mths = 1; // Because months are 1-indexed
	if(total >= daycounts[0]){
		total -= daycounts[0]; // Subtract January if appropriate
		mths++;
		
		if(total >= daycounts[1]){
			total -= daycounts[1]; // Subtract February if appropriate
			if((daycounts > 0) && isLeapYear(yrs))
				total--;
			mths++;
			
			while(mths < (MONTHS_PER_YEAR+1) && total >= daycounts[mths-1]){
				total -= daycounts[mths-1];
				mths++;
			}
		}
	}
	
	total++; // Because days are 1-indexed

	int hours = secs / (MINUTES_PER_HOUR * SECONDS_PER_MINUTE);
	secs %= (MINUTES_PER_HOUR * SECONDS_PER_MINUTE);
	int mins = secs / (SECONDS_PER_MINUTE);
	secs %= (SECONDS_PER_MINUTE);
	
	result << yrs << datesep << mths << datesep << total << SPACE;
	
	// Some prettying-up
	if(hours < 10)
		result << '0';
	result << hours << timesep;
	if(mins < 10)
		result << '0';
	result << mins << timesep;
	if(secs < 10)
		result << '0';
	result << secs;
	
	return result.str();
}
