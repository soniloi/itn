/* Class handling the categorisation and initial parsing of various types of input lines
 */

#include <string>
#include <map>
#include "Config.h"
#include "Time.h"

typedef enum {
	TOPICCHANGE,
	NICKCHANGE,
	JOIN,
	PART,
	QUIT,
	KILL,
	KICK,
	MODECHANGE,
	UNDEF
} accidentaltype;

typedef struct activitylineinfo{
	Time time;
	std::string nick;
	unsigned char initial; // First character of the comment part of the line
} activitylineinfo;

typedef struct accidental{
	accidentaltype type; // The type of the accidental
	int index; // The index in the string for where parsing should resume
} accidental;

typedef struct nickline{
	std::string nick;
	std::string rest;
} nickline;

class LineParser{

private:
	static std::map<std::string, int> makeMap(){
		std::map<std::string, int> m;
		m["Jan"] = 0;
		m["Feb"] = 1;
		m["Mar"] = 2;
		m["Apr"] = 3;
		m["May"] = 4;
		m["Jun"] = 5;
		m["Jul"] = 6;
		m["Aug"] = 7;
		m["Sep"] = 8;
		m["Oct"] = 9;
		m["Nov"] = 10;
		m["Dec"] = 11;
		return m;
	}
	static std::map<std::string, int> months;

public:
	static Time getTimeFromInfoLine(std::string line, successcode &code);
	static Time getTimeFromTimestampedLine(std::string line, int y, int mo, int d, successcode &code);
	static accidental findAccidentalType(std::string line, int index);
	static nickline splitMeLine(std::string line, int index, successcode &code);
	static nickline splitCommentLine(std::string line, int index, successcode &code);
	static int calculateBotcount(activitylineinfo current, activitylineinfo previous);
};
